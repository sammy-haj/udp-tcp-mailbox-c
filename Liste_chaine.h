//On définit la structure lettre
	typedef struct lettre lettre;
	struct lettre{
		char *message; //Message stocké dans la lettre			
		struct lettre *suiv; //Adresse de la lettre suivante
	};

	//On définit la structure qui controle la Liste des lettres associé à une BAL
	typedef struct Liste_Controle_Lettre Liste_Controle_Lettre;
	struct Liste_Controle_Lettre
	{
		lettre *premier; //Adresse de la première lettre dans la BAL correspondante
	};

	//On définit la structure BAL
	typedef struct BAL BAL;
	struct BAL{
		struct Liste_Controle_Lettre *Liste_Controle_Lettre;   //Structure qui controle la Liste des lettres associé à une BAL
		struct BAL *suiv; //Adresse de la BAL suivante
		int id_R;   //Identifiant du récepteur
	};

	//On définit la structure qui controle la Liste des BAL
	typedef struct Liste_Controle_BAL Liste_Controle_BAL;
	struct Liste_Controle_BAL
	{
		BAL *premier; //Adresse de la première BAL
	};

Liste_Controle_BAL * initialisation_Liste_Controle_BAL();
void insertion_BAL(Liste_Controle_BAL * liste_BAL,int numeroRecep,Liste_Controle_Lettre * liste_des_lettres);
Liste_Controle_Lettre * initialisation_Liste_Controle_Lettre();
void insertion_Lettre(Liste_Controle_Lettre * Liste_Controle_L,char * M,int lg_message);
void BAL_Reception_Emetteur(int nb_message,int sock,int sock_bis,int lg_message,int numero_emetteur,Liste_Controle_Lettre *Liste_Controle_Lettre_Actuel,char *M);
void BAL_Emission_Recepteur(int nbLettres,int sock,int sock_bis,int lg_message,char ** lettres,int numero_recepteur);
void Stockage_Lettres(lettre *courant,int nbLettres,int sock,char** lettres);
int Parcourir_BAL(Liste_Controle_BAL * maListe_Controle_Bal_Temp,int numero_recepteur,int sock,int sock_bis);
Liste_Controle_Lettre * Creation_BAL(Liste_Controle_BAL *maListe_Controle_Bal_Temp, int numero_emetteur,Liste_Controle_Lettre * Liste_Controle_Lettre_Actuel,Liste_Controle_BAL *maListe_Controle_Bal);