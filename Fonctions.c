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

//Fonction affichage et construction
void construire_message (char *message, char motif, int lg,int numero) {
	int i;
	for (i=0 ; i<5; i++){
		message[i]='-';
	}
	sprintf(message,"%5d",numero);
	for (i=5 ; i<lg; i++){
	message[i] = motif; 
	}
	}

void afficher_message (char *message, int lg) {
	int i;
	for (i=0 ; i<lg ; i++) printf("%c", message[i]); 
}

//Création du socket
int Creation_Socket (){
    int sock;
	if ((sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))==-1){
		printf("Echec de creation du socket\n");
		close(sock);
	    exit(1);
		}
    return sock;
}

void Associe_Socket(int sock,struct sockaddr_in adr_local,int lg_adr_local){
    	//Permet de réutiliser directement le port même après une interruption avec (Ctrl+C)
		int yes=1;
		if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
			perror("setsockopt");
			close(sock);
			exit(1);
		}
		//On associe l’adresse au socket
		if (bind(sock,(struct sockaddr*)&adr_local,lg_adr_local)==-1){
			printf("echec du bind\n");
			close(sock);
			exit(1);
		}
}

void Construit_Adresse(int sock,struct sockaddr_in adr_distant,char **argv,int argc){
    struct hostent *hp;
    //On construit l’adresse du socket destinataire
	memset((char *)& adr_distant,0,sizeof(adr_distant));
    adr_distant.sin_family=AF_INET;
	adr_distant.sin_port=htons(atoi(argv[argc-1])); 

    if ((hp=gethostbyname(argv[argc-2]))==NULL){
		printf("erreur gethostbyname\n");
		close(sock);
		exit(1);
	    }
	memcpy((char*)&(adr_distant.sin_addr.s_addr),
	hp->h_addr,
    hp->h_length);
}

void Reception_Message_Recepteur(int sock,int lg_message, int numero_recepteur, char *M)
{
    int lg_rec;
    int i = 0;
    while (1)
    {
        if ((lg_rec = read(sock, M, lg_message)) < 0)
        {
            printf("échec du read\n");
            close(sock);
            exit(1);
        }
        if (lg_rec >= 1)
        {
            printf("RECEPTION : Récupération lettre n°%d par le récepteur %d (%d) [", i, numero_recepteur, lg_message);
            afficher_message(M, lg_message);
            printf("]\n");
            i++;
        }
        else
        {
            break;
        }
    }
}

void Envoi_Message_Emetteur(int sock,int lg_message,int nb_message,int numero_emetteur,struct sockaddr_in adr_distant,char **argv,int argc,int lg_adr_distant, char *M){
    char lettre = 'a';
    int lg_emis;
    //On affiche les paramètres de l'envoi
    printf("SOURCE : lg_mesg_emis= %d , port= %s , nb_envois= %d , TP=tcp, dest= %s\n",lg_message,argv[argc-1],nb_message,argv[argc-2]);

    //On construit le message et on l'envoi
    for (int i=0;i < nb_message;i++){
        //On construit le message
        construire_message(M,lettre,lg_message,i);
        //On envoi le message
        if ((lg_emis=sendto(sock,M,lg_message,0, (struct sockaddr*)&adr_distant,lg_adr_distant))==-1){
            printf("erreur d'envoi\n");
            close(sock);
            exit(1);
            }
        //On affiche le message envoyé
        printf("SOURCE: Envoi lettre n°%d à destination du récepteur %d (%d) [",i,numero_emetteur,lg_message);
        afficher_message(M,lg_message);
        printf("]\n");
        if (lettre==('z')){
            lettre='a';
        }else{
            lettre++;
            }
        }
        printf("SOURCE: fin\n");
    }