#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Declaration des constantes
#define PORT 1000 //port de connexion
#define MAX_BUFFER 1000 //buffer
#define boolean int //type booleen
#define true 1 //
#define false 0 //
#define MAX_PLACES 100 //nombre maximum de places

//Delcaration des structures
typedef struct{
    boolean instanceOf; //boolean permettant de savoir si la structure actuelle est utilisée ou non

    int places_occupees; //nombre de places occupées
    int places_libres; //nombre de places libres dans la salle
} salle;

typedef struct{
    boolean instanceOf; //boolean permettant de savoir si la structure actuelle est utilisée ou non

    int no_dossier; //numéro du dossier
    char nom[100]; //nom de la personne associée au numéro de dossier
} dossier;

typedef struct{
    boolean instanceOf; //boolean permettant de savoir si la structure actuelle est utilisée ou non

    char adresse_client[100]; //adresse du client
    int port; //port

    int index; //index du tableau
    int fdSocketAttente;
    int fdSocketCommunication;
} communication;

//declaration des fonctions
void creationDossier(dossier* d);
void initSalle(salle* s);
void initCommunication(communication* c);