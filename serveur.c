#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <pthread.h>

//declaration des constantes
#define PORT 3000 //port de connexion
#define MAX_BUFFER 1000 //buffer
#define boolean int //type booleen
#define true 1 //
#define false 0 //
#define MAX_PLACES 100 //nombre maximum de places

//delcaration des structures
typedef struct{
    boolean instanceOf; //boolean permettant de savoir si la structure actuelle est utilisée ou non

    int placesOccupees; //nombre de places occupées
    int placesLibres; //nombre de places libres dans la salle
} salle;

typedef struct{
    boolean instanceOf; //boolean permettant de savoir si la structure actuelle est utilisée ou non

    int noDossier; //numéro du dossier
    char nom[100]; //nom de la personne associée au numéro de dossier
    char prenom[100]; //prenom de la personne associé au numéro de dossier
} dossier;

typedef struct{
    boolean instanceOf; //boolean permettant de savoir si la structure actuelle est utilisée ou non

    char adresseClient[100]; //adresse du client
    int port; //port

    int index; //index du tableau
    int fdSocketAttente;
    int fdSocketCommunication;
} communication;

//declaration des fonctions
void creationDossier(dossier* d);
void initSalle(salle* s);
void initCommunication(communication* c);
void afficherMenuPrincipal();
void afficherMenuInscription();
void afficherMenuDesinscription();

void affichereMenuPrincipal(){
    printf("Bienvenue:\nVeuillez entrer le votre choix à l'aide du numéro associé à celui-ci.\n");
    printf("1. S'inscrire");
    printf("2. Se désinscrire");
    printf("3. Quitter\n");
}

void afficherMenuInscription(){
    printf("Inscription:\n");
    printf("Veuillez saisir votre nom puis votre prenom\n");
}

void afficherMenuDesinscription(){
    printf("Désinscription:\n");
    printf("Veuillez saisir le numéro de dossier qui vous a été donné lors de l'inscription.");
    printf("Ce numéro de dossier est composé de 10 chiffres.\n");
}

//main program
int main(int argc, char const *argv[])
{
    //initialisation des variables
    int fdSocketAttente;
    int fdSocketCommunication;
    struct sockaddr_in coordServeur;
    struct sockaddr_in coordClient;
    int longueurAdresse;
    int nbRecu;
    int pid;

    //initialisation de la socket et test si elle est correcte avant de continuer
    fdSocketAttente = socket(PF_INET, SOCK_STREAM, 0);
    if(fdSocketAttente < 0) {
        printf("Erreur ! Socket incorrecte\n");
        exit(EXIT_FAILURE);
    }

    //preparation de l'adresse d'attachement locale
    longueurAdresse = sizeof(struct sockaddr_in);
    memset(&coordServeur, 0x00, longueurAdresse);

    coordServeur.sin_family = PF_INET;
    //interfaces locales dispo pour l'adresse
    coordServeur.sin_addr.s_addr = htonl(INADDR_ANY);
    //interfaces locales dispo pour le port
    coordServeur.sin_port = htons(PORT);

    //test du bind avant de continuer
    if(bind(fdSocketAttente, (struct sockaddr *) &coordServeur, sizeof(coordServeur)) == -1){
        printf("Erreur ! Bind incorrect\n");
        exit(EXIT_FAILURE);
    }

    //test du listen avant de continuer
    if(listen(fdSocketAttente, 5) == -1){
        printf("Erreur ! Listen incorrect");
        exit(EXIT_FAILURE);
    }

    printf("Connexion en cours\n");
    printf("Pour voir les clients connectés ainsi que leur status, appuyez sur Entrée\n");

    //affichage
    socklen_t tailleCoord = sizeof(coordClient);
    pthread_t threadAffichage;
}