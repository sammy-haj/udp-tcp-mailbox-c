/* librairie standard ... */
#include <stdlib.h>
/* pour getopt */
#include <unistd.h>
/* déclaration des types de base */
#include <sys/types.h>
/* constantes relatives aux domaines, types et protocoles */
#include <sys/socket.h>
/* constantes et structures propres au domaine UNIX */
#include <sys/un.h>
/* constantes et structures propres au domaine INTERNET */
#include <netinet/in.h>
/* structures retournées par les fonctions de gestion de la base de
données du réseau */
#include <netdb.h>
/* pour les entrées/sorties */
#include <stdio.h>
/* pour la gestion des erreurs */
#include <errno.h>
/* Pour la gestion des signaux (Ctrl+C)*/
#include <signal.h> 
/* Gère le fonctionnement des liste chainées*/
#include "Liste_chaine.h"
/* Toutes les fonctions utiles*/
#include "Fonctions.h"

//Fonction permettant d'initialiser la structure qui controle la Liste des BAL
	Liste_Controle_BAL * initialisation_Liste_Controle_BAL()
	{
		//On réserve l'espace
		Liste_Controle_BAL *Liste_Controle_BAL = malloc(sizeof(*Liste_Controle_BAL));
		if (Liste_Controle_BAL == NULL)
		{
			exit(EXIT_FAILURE);
		}
		//Le première élement de la liste des BAL est NULL
		Liste_Controle_BAL->premier=NULL;

		return Liste_Controle_BAL;
	}

	//Fonction permettant d'insérer une BAL
	void insertion_BAL(Liste_Controle_BAL * liste_BAL,int numeroRecep,Liste_Controle_Lettre * liste_des_lettres)
	{
		//On réserve l'espace
		BAL *nouveau = malloc(sizeof(*nouveau));
		if (liste_BAL == NULL || nouveau == NULL)
    	{
        	exit(EXIT_FAILURE);
   		}				
		//On associe le numéro de réception à id_R et on associe à la BAL sa structure qui controle la Liste des lettres
		nouveau->id_R=numeroRecep;
		nouveau->Liste_Controle_Lettre=liste_des_lettres;

		//On place la BAL au debut de notre structure qui controle la liste des BAL, l'ancienne première BAL est placée après la nouvelle BAL
		nouveau->suiv=liste_BAL->premier;
		liste_BAL->premier=nouveau;
	}

	//Fonction permettant d'initialiser la structure qui controle la Liste des lettres associés à une BAL
	Liste_Controle_Lettre * initialisation_Liste_Controle_Lettre()
	{
		//On réserve l'espace
		Liste_Controle_Lettre *Liste_Controle_L = malloc(sizeof(*Liste_Controle_L));

		if (Liste_Controle_L == NULL)
		{
			exit(EXIT_FAILURE);
		}
		//Le première élement de la liste des lettres est NULL
		Liste_Controle_L->premier=NULL;

		return Liste_Controle_L;
	}

	//Fonction permettant d'insérer une lettre dans la liste des lettres associés à une BAL
	void insertion_Lettre(Liste_Controle_Lettre * Liste_Controle_L,char * M,int lg_message)
	{
		//On réserve l'espace
		lettre *nouveau = malloc(sizeof(*nouveau));
    	if (nouveau == NULL) {
        	exit(EXIT_FAILURE);
    	}

    	//On réserve l'espace pour le message
    	nouveau->message = malloc(lg_message * sizeof(char));
    	if (nouveau->message == NULL) {
        	free(nouveau); // Libération de la mémoire allouée pour la lettre si l'allocation échoue
        	exit(EXIT_FAILURE);
    	}

    	//Copie des données dans le message
    	strncpy(nouveau->message, M, lg_message);
    	nouveau->message[lg_message - 1] = '\0';

		//On place la lettre au debut de notre structure qui controle la liste des lettres, l'ancienne première lettre est placée après la nouvelle lettre
		nouveau->suiv=Liste_Controle_L->premier;
		Liste_Controle_L->premier=nouveau;
	}

void BAL_Reception_Emetteur(int nb_message,int sock,int sock_bis,int lg_message,int numero_emetteur,Liste_Controle_Lettre *Liste_Controle_Lettre_Actuel,char *M){
    int i=0;
	int lg_rec;
    while (i<nb_message){
        
        //On invoque la primitive de réception de message pour pouvoir recevoir les messages
        if((lg_rec=read(sock_bis,M,lg_message))<0){
            printf("échec du read\n");
            close(sock);
            exit(1);
            }
        //Si on reçoit bien des données
        else if (lg_rec>=1){
            //On stock les lettres dans la structure permettant de gérer les lettres de la BAL
            insertion_Lettre(Liste_Controle_Lettre_Actuel,M,lg_message);
            //On affiche les messages reçus
            printf("PUITS : Réception et stockage lettre n°%d pour le récepteur n°%d [",i,numero_emetteur);
            afficher_message(M,lg_message);
            printf("]\n");
            i++;
            }
        }
    }

void BAL_Emission_Recepteur(int nbLettres,int sock,int sock_bis,int lg_message,char ** lettres,int numero_recepteur){
    for (int j = nbLettres - 1; j >= 0; j--) {
        if ((write(sock_bis, lettres[j], lg_message)) == -1) {
            printf("Erreur envoi de la lettre au récepteur\n");
            close(sock);
            free(lettres); 
            exit(1);
        }

        //On affiche les messages envoyer et destocké
        printf("PUITS : Envoi et déstockage lettre n°%d pour le récepteur n°%d [", nbLettres - j -1, numero_recepteur);
        afficher_message(lettres[j], lg_message);
        printf("]\n");
    }
    free(lettres);
}

void Stockage_Lettres(lettre *courant,int nbLettres,int sock,char **lettres){
	lettre *suivant;
    if (lettres == NULL) {
        printf("Erreur d'allocation mémoire\n");
        close(sock);
        exit(1);
    }
    int i = 0;
    while (courant != NULL) {
        lettres[i] = courant->message;
        i++;
        suivant = courant->suiv;
        free(courant); 
        courant = suivant;
    }
}

//Tant que id_R ne correspond pas au numero du récepteur on parcours avec la structure temporaire la liste des BAL
int Parcourir_BAL(Liste_Controle_BAL * maListe_Controle_Bal_Temp,int numero_recepteur,int sock,int sock_bis){
    while (maListe_Controle_Bal_Temp->premier->id_R!=numero_recepteur){ 
        
        //Si il n'y a aucune BAL correspondant au numéro de récepteur on ferme la connexion
        if (maListe_Controle_Bal_Temp->premier->suiv==NULL){
            printf("SOURCE: Il n'y a pas de lettre pour le Recepteur %d\n",numero_recepteur);

            if (shutdown(sock_bis,2)==-1){
                printf("Erreur shutdown\n");
                close(sock);
                exit(1);
            }

            //Permet de quitter la fonction
            return 1;
            }
			
        //Permet de parcourir la liste des BAL
        maListe_Controle_Bal_Temp->premier=maListe_Controle_Bal_Temp->premier->suiv;
        }
	return 0;
}

Liste_Controle_Lettre * Creation_BAL(Liste_Controle_BAL *maListe_Controle_Bal_Temp, int numero_emetteur,Liste_Controle_Lettre * Liste_Controle_Lettre_Actuel,Liste_Controle_BAL *maListe_Controle_Bal){
	//On regarde si la liste des BAL est vide
	if (maListe_Controle_Bal_Temp->premier!=NULL){
		//On regarde si la BAL existe déjà
		while (maListe_Controle_Bal_Temp->premier->id_R!=numero_emetteur){ 
			printf("OK\n");
			//Si elle n'existe pas on la créer
			if (maListe_Controle_Bal_Temp->premier->suiv==NULL){
				//On créer la structure permettant de gérer les lettres de la BAL
				Liste_Controle_Lettre_Actuel=initialisation_Liste_Controle_Lettre();
				//On créer une nouvelle BAL dans la liste de gestion des BAL avec le numéro émetteur et liste de controle des lettres qu'on vient de créer
				insertion_BAL(maListe_Controle_Bal,numero_emetteur,Liste_Controle_Lettre_Actuel);
				return Liste_Controle_Lettre_Actuel;
			}

			//Permet de parcourir la liste des BAL
			maListe_Controle_Bal_Temp->premier=maListe_Controle_Bal_Temp->premier->suiv;
		}

		//Si la BAL existe déja on la selectionne
		if(maListe_Controle_Bal_Temp->premier->id_R==numero_emetteur){
			Liste_Controle_Lettre_Actuel=maListe_Controle_Bal_Temp->premier->Liste_Controle_Lettre;
			return Liste_Controle_Lettre_Actuel;
		}
	}
	else{
		//On créer la structure permettant de gérer les lettres de la BAL
		Liste_Controle_Lettre_Actuel=initialisation_Liste_Controle_Lettre();
		//On créer une nouvelle BAL dans la liste de gestion des BAL avec le numéro émetteur et liste de controle des lettres qu'on vient de créer
		insertion_BAL(maListe_Controle_Bal,numero_emetteur,Liste_Controle_Lettre_Actuel);
		return Liste_Controle_Lettre_Actuel;
	}
	return Liste_Controle_Lettre_Actuel;
}