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
/* Toutes les fonctions utiles*/
#include "Fonctions.h"
/* Gère le fonctionnement des liste chainées*/
#include "Liste_chaine.h"

int sock;

//Fermeture du socket si interruption avec (Ctrl+C)
void handle_sigint(int sig) {
    
    
    // Fermeture du socket et de la connexion si le descripteur de socket est valide
	if (shutdown(sock,2)==-1){
		printf("Erreur shutdown\n");
		close(sock);
		exit(1);
	}

    if (sock != -1) {
        close(sock);
    }
	printf("\n(Ctrl+C) reçu. Fermeture du socket et sortie.\n");

    exit(0);
}

int main (int argc, char **argv)
{
	//Fonction appelé pour gérer le signal (Ctrl+C)
	signal(SIGINT, handle_sigint);

	int c;
	extern char *optarg;
	extern int optind;
	int nb_message = 10; /* Nb de messages à envoyer ou à recevoir, par défaut : 10 en émission, infini en réception */
	int lg_message = 30; /* Lg de messages à envoyer ou à recevoir, par défaut : 30 en émission, 30 en réception */
	int numero_emetteur;
	int numero_recepteur;
    int type = -1; /*0=Recepteur  1=Emetteur  2=BoiteAuxLettres*/
	//On récupère les options
	while ((c = getopt(argc, argv, "br:e:l:n:")) != -1) { 
		switch (c) {
		case 'n':
			nb_message = atoi(optarg);
			break;

		case 'l':
			lg_message = atoi(optarg);
			break;

        case 'e':
            type=2;
            if((numero_emetteur=atoi(optarg))==0){
				numero_emetteur=1;
			}
            break;

        case 'r':
            type=1;
            if((numero_recepteur=atoi(optarg))==0){
				numero_recepteur=1;
			};
            break;

        case 'b':
            type=3;
            break;

		default:
			printf("usage: cmd [-p|-s][-n ##]\n");
			break;
		}
	}

	struct sockaddr_in adr_distant;

	if (type==1){                     //Recepteur   

        adr_distant.sin_family=AF_INET;
	    adr_distant.sin_port=htons(atoi(argv[argc-1]));

		//Création du socket
		sock=Creation_Socket();

		//On construit l’adresse du socket destinataire
        Construit_Adresse(sock,adr_distant,argv,argc);

		//On demande l’établissement d’une connexion entre le socket créé et le socket de la BAL
        if (connect(sock,(struct sockaddr*)&adr_distant,sizeof(adr_distant))==-1){
            printf("erreur connect\n");
			close(sock);
		    exit(1);
            }

		//On s'identifie auprès de la BAL pour cela on définit les informations du PDU et sa taille
		int lg_PDU=4;
		int PDU[lg_PDU];
        int lg_emis;
		PDU[0]=1;
		PDU[1]=numero_recepteur;

		//On envoi le PDU
		if ((lg_emis=write(sock,PDU,sizeof(PDU)))==-1){
            printf("Erreur envoi PDU\n");
			close(sock);
            exit(1);
            }

		//On définit la taille du message
		char M[lg_message];

		//On récupère les messages correspondant au numéro de recepteur à la BAL
		Reception_Message_Recepteur(sock,lg_message,numero_recepteur,M);
				
				//On ferme la connexion
				if (shutdown(sock,2)==-1){
                    printf("Erreur shutdown\n");
					close(sock);
                    exit(1);
				}	
    }         

	else if(type==2){                  //Emetteur

	    adr_distant.sin_family=AF_INET;
	    adr_distant.sin_port=htons(atoi(argv[argc-1]));

		//Création du socket
		sock=Creation_Socket();  

		//On construit l’adresse du socket destinataire
        Construit_Adresse(sock,adr_distant,argv,argc);

		//On demande l’établissement d’une connexion entre le socket créé et le socket de la BAL
        if (connect(sock,(struct sockaddr*)&adr_distant,sizeof(adr_distant))==-1){
            printf("erreur connect\n");
			close(sock);
		    exit(1);
            }

		//On s'identifie auprès de la BAL pour cela on définit les informations du PDU, sa taille et la taille du message à envoyer
		int lg_PDU=4;
		int PDU[lg_PDU];
        int lg_emis;
        int lg_adr_distant=sizeof(adr_distant);
        char M[lg_message];

		PDU[0]=0;
		PDU[1]=numero_emetteur;
		PDU[2]=nb_message;
		PDU[3]=lg_message;

		//On envoi le PDU
		if ((lg_emis=write(sock,PDU,sizeof(PDU)))==-1){
            printf("Erreur envoi PDU\n");
			close(sock);
            exit(1);
			}

		//On construit le message et on l'envoi
		Envoi_Message_Emetteur(sock,lg_message,nb_message,numero_emetteur,adr_distant,argv,argc,lg_adr_distant, M);
			
			//On ferme la connexion
			if (shutdown(sock,2)==-1){
                printf("Erreur shutdown\n");
				close(sock);
                exit(1);
			}
    }




	else if(type==3){                 //BAL

		//On créer notre structure nous permettant de gérer la liste des BAL
		Liste_Controle_BAL *maListe_Controle_Bal = initialisation_Liste_Controle_BAL();

		//On crée un socket local 
		sock=Creation_Socket();

		int sock_bis;
		struct sockaddr_in adr_client;
		unsigned int lg_adr_client=sizeof(adr_client);

	//On définit la longueur du message, et du PDU
		int lg_rec_PDU;
		char M[lg_message];
		int lg_PDU=4;
		int PDU[lg_PDU];
		struct sockaddr_in adr_local;
		int lg_adr_local=sizeof(adr_local);
		memset((char*)&adr_local,0,sizeof(adr_local));
		adr_local.sin_family=AF_INET;
		adr_local.sin_port=htons(atoi(argv[argc-1]));
		adr_local.sin_addr.s_addr=INADDR_ANY;
		
		//Permet de réutiliser directement le port même après une interruption avec (Ctrl+C) et on associe l’adresse au socket
		Associe_Socket(sock,adr_local,lg_adr_local);

		//On boucle à l'infini
		while(1){

			//On affiche les paramètres de la BAL
			printf("\nPUITS: port=%s, TP=tcp\n",argv[argc-1]);
			
			//On se met en état d’acceptation de connexion de tout demande de connexion
			listen(sock,5);

			if ((sock_bis=accept(sock,(struct sockaddr*)&adr_client,&lg_adr_client))==-1){
				printf("échec du accept\n");
				close(sock);
				exit(1);
			}

			//On invoque la primitive de réception de message pour pouvoir recevoir le PDU
			if((lg_rec_PDU=read(sock_bis,PDU,sizeof(PDU)))<0){
					printf("échec du read\n");
					close(sock);
					exit(1);
				}
				
			//PDU[0]=0 BAL en réception d'un émetteur
			//PDU[0]=1 //BAL retransmet au recepteur

			if (PDU[0]==0){                //BAL en réception d'un émetteur

				//On récupère les données dans le PDU
				numero_emetteur=PDU[1];
				nb_message=PDU[2];
				lg_message=PDU[3];
				
				//On affiche les informations des messages que la BAL va recevoir de l'émetteur
				printf("Emetteur n°%d Envoi de %d messages de longueur %d\n",numero_emetteur,nb_message,lg_message);

				//On créer notre structure temporaire nous permettant de gérer la liste des BAL
				Liste_Controle_BAL *maListe_Controle_Bal_Temp = malloc(sizeof(Liste_Controle_BAL));
				
				//Cette structure temporaire pointe vers le même endroit que celle original
				maListe_Controle_Bal_Temp->premier=maListe_Controle_Bal->premier;

				//On créer la structure permettant de gérer les lettres de la BAL
				Liste_Controle_Lettre * Liste_Controle_Lettre_Actuel=Creation_BAL(maListe_Controle_Bal_Temp, numero_emetteur,Liste_Controle_Lettre_Actuel,maListe_Controle_Bal);
				
				//On récupère les messages et on les stock
				BAL_Reception_Emetteur(nb_message,sock,sock_bis,lg_message,numero_emetteur,Liste_Controle_Lettre_Actuel,M);
				}


			else if (PDU[0]==1){              //BAL retransmet au recepteur

				//On récupère les données dans le PDU
				numero_recepteur=PDU[1];

				//On créer notre structure temporaire nous permettant de gérer la liste des BAL
				Liste_Controle_BAL *maListe_Controle_Bal_Temp = malloc(sizeof(Liste_Controle_BAL));
				
				//Cette structure temporaire pointe vers le même endroit que celle original
				maListe_Controle_Bal_Temp->premier=maListe_Controle_Bal->premier;

				//Si il n'y a aucune BAL de créer on ferme la connexion
				if (maListe_Controle_Bal_Temp->premier==NULL){
					printf("SOURCE: Il n'y a pas de lettre pour le Recepteur %d\n",numero_recepteur);

					if (shutdown(sock_bis,2)==-1){
						printf("Erreur shutdown\n");
						close(sock);
						exit(1);
					}
					//Permet de reboucler au début de la boucle do while
					continue;
					}

				//On trouve la BAL correspondant au numéro du recepteur
				if ((Parcourir_BAL(maListe_Controle_Bal_Temp,numero_recepteur,sock,sock_bis))==1){
					//Permet de reboucler au début de la boucle do while
					continue;
				}


				// On initialise des structure lettres courant et suivant correspondant respectivement à la première lettre de la BAL et à la suivante
				lettre *courant = maListe_Controle_Bal_Temp->premier->Liste_Controle_Lettre->premier;
				
				int nbLettres=0;

				//Si il n'y a pas de lettre
				if (courant==NULL){
					printf("SOURCE: Il n'y a pas de lettre pour le Recepteur %d\n",numero_recepteur);

					if (shutdown(sock_bis,2)==-1){
						printf("Erreur shutdown\n");
						close(sock);
						exit(1);
					}
					//Permet de reboucler au début de la boucle do while
					continue;
				} 
				
				//Tant qu'on a pas parcouru toute les lettres
				while (courant != NULL) {
					//On compte le nombre de lettres et on parcours les lettres
					nbLettres++;
					courant = courant->suiv;
				}
				
				//On rédefinit courant comme étant le premier message de la BAL
				courant = maListe_Controle_Bal_Temp->premier->Liste_Controle_Lettre->premier;

				//On réserve de l'espace pour stocké les lettres
   				char **lettres = (char **)malloc(nbLettres * sizeof(char *));

				//Tant qu'on a pas parcouru toute les lettres on stock les lettre dans un tableau
				Stockage_Lettres(courant,nbLettres,sock,lettres);

				//On supprime les lettres stockés de la BAL
				maListe_Controle_Bal_Temp->premier->Liste_Controle_Lettre->premier = NULL;
			
				//On envoie dans le sens inverse les messages au récepteur
				BAL_Emission_Recepteur(nbLettres,sock,sock_bis,lg_message,lettres,numero_recepteur);

				//On ferme la connexion
				if (shutdown(sock_bis,2)==-1){
                printf("Erreur shutdown\n");
				close(sock);
                exit(1);
				}

				}               
		}
	}
	return 0;
}
