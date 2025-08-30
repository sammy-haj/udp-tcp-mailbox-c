void construire_message (char *message, char motif, int lg,int numero);
void afficher_message (char *message, int lg);
int Creation_Socket ();
void Associe_Socket(int sock,struct sockaddr_in adr_local,int lg_adr_local);
void Construit_Adresse(int sock,struct sockaddr_in adr_distant,char **argv,int argc);
void Reception_Message_Recepteur(int sock,int lg_message,int numero_recepteur,char *M);
void Envoi_Message_Emetteur(int sock,int lg_message,int nb_message,int numero_emetteur,struct sockaddr_in adr_distant,char **argv,int argc,int lg_adr_distant, char *M);