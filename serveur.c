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
#define null NULL //null
#define TAILLE_NO_DOSSIER 10 //taille du numero dossier

//delcaration des structures
typedef struct{
    int placesLibres; //nombre de places libres dans la salle
} Salle;

typedef struct{
    int noDossier; //numéro du dossier
    char nom[20]; //nom de la personne associée au numéro de dossier
    char prenom[20]; //prenom de la personne associé au numéro de dossier
} Dossier;

typedef struct{
    Dossier dossier;
    struct CaseDossier * dossierSuivant;
} CaseDossier;
typedef CaseDossier * listeDossiers;

listeDossiers liste; //creation d'une liste des dossiers, et donc du nombre de place dispo.

//declaration des fonctions
Dossier* creationDossier(char nom[20], char prenom[20]);
void initSalle(Salle* s);
void afficherMenuPrincipal();
void afficherMenuInscription();
void afficherMenuDesinscription();
int randint(int bi, int bs);
int numeroDossier();
void ajoutDossierEnTete(listeDossiers liste, Dossier d);
void rechElement(listeDossiers liste, Dossier d);
listeDossiers creerCase(Dossier d);

void afficherMenuInscription(int fdSocketCommunication){
    //declaration des variables de travail
    char text[100];
    int nbRecu;
    char nom[20], prenom[20];
    Dossier* d;

    //envoi d'une demande de saisie du nom au client
    strcpy(text, "Inscription:\nVeuillez saisir votre nom:\n");
    send(fdSocketCommunication, text, strlen(text), 0);

    //reception de la saisie
    nbRecu = recv(fdSocketCommunication, text, 99, 0);
    if(nbRecu > 0){
        text[nbRecu] = 0;
        printf("Reçu: %s\n", text);
        //stockage du nom
        *nom = *text;
    } else {
        printf("Erreur");
    }

    //envoi d'une demande de saisie du prenom au client
    strcpy(text, "Veuillez saisir votre prenom:\n");
    send(fdSocketCommunication, text, strlen(text), 0);
    nbRecu = recv(fdSocketCommunication, text, 99, 0);
    if(nbRecu > 0){
        text[nbRecu] = 0;
        printf("Reçu: %s\n", text);
        //stockage du prenom
        *prenom = *text;
    } else {
        printf("Erreur");
    }

    d = creationDossier(nom, prenom);

}

//fonction menu desinscription
void afficherMenuDesinscription(){
    printf("Désinscription:\n");
    printf("Veuillez saisir le numéro de dossier qui vous a été donné lors de l'inscription.");
    printf("Ce numéro de dossier est composé de 10 chiffres.\n");
}

//fonction menu principal
void menuPrincipal(){
    printf("Bienvenue:\nVeuillez entrer le votre choix à l'aide du numéro associé à celui-ci.\n");
    printf("1. S'inscrire\n");
    printf("2. Se désinscrire\n");
    printf("3. Quitter\n");
}

//fonction de creation de dossier
Dossier* creationDossier(char nom[20], char prenom[20]){
    Dossier* d;
    *d->nom = *nom;
    *d->prenom = *prenom;
    d->noDossier = numeroDossier();
    

    return d;
}

//generation d'un numero de dossier
int numeroDossier(){
    int no;
    int coef = 1;
    for(int i = 0; i < TAILLE_NO_DOSSIER; i++){
        int n = randint(0,9);
        no += n*coef;
        coef *= 10;
    }
    return no;
}

//retourn un numero aleatoire
int randint(int bi, int bs){
    int n;
    srand(time(null));
    n = rand() % bs + bi;
    return n;
}

void ajoutDossierEnTete(listeDossiers L, Dossier d){
    listeDossiers P = creerCase(d);
    P->dossierSuivant = L;
    L = P;
}

listeDossiers creerCase(Dossier d){
    listeDossiers L;
    L->dossier = d;
    L->dossierSuivant = null;
    return L;
}

void rechElement(listeDossiers liste, Dossier d){

}

//main program
int main(int argc, char const *argv[])
{
    //déclaration des variables
    int fdSocketAttente;
    int fdSocketCommunication;
    struct sockaddr_in coordServeur;
    struct sockaddr_in coordClient;
    int longueurAdresse;
    int nbRecu;
    char tampon[100];
    Salle* salle;
    salle->placesLibres = MAX_PLACES - sizeof(*liste);

    //initialisation de la socket et test si elle est correcte avant de continuer
    fdSocketAttente = socket(PF_INET, SOCK_STREAM, 0);
    if(fdSocketAttente < 0) {
        printf("Erreur ! Socket incorrecte\n");
        exit(EXIT_FAILURE);
    } else {
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
        } else if(listen(fdSocketAttente, 5) == -1){ //test du listen avant de continuer
            printf("Erreur ! Listen incorrect\n");
            exit(EXIT_FAILURE);
        } else {
            //affichage
            socklen_t tailleCoord = sizeof(coordClient);
            /*
            pthread_t threadAffichage;
            pthread_create(&threadAffichage, null, null, null);
            */

            while(true){
                printf("En attente de connexion..\n");
                //test accept
                if((fdSocketCommunication = accept(fdSocketAttente, (struct sockaddr *) &coordClient, &tailleCoord)) == -1){
                    printf("Erreur ! Accept incorrect\n");
                    exit(EXIT_FAILURE);
                } else {
                    printf("Client connecté !\n");
                    printf("Adresse: %s:%d\n", inet_ntoa(coordClient.sin_addr), ntohs(coordClient.sin_port));

                    nbRecu = recv(fdSocketCommunication, tampon, 99, 0);
                    if(nbRecu > 0){
                        tampon[nbRecu] = 0;
                        printf("Reçu: %s\n", tampon);
                    } else {
                        printf("Erreur");
                    }

                    //Envoie d'un msg au client
                    strcpy(tampon, "Message bien reçu!\n");
                    send(fdSocketCommunication, tampon, strlen(tampon), 0);
                    printf("Connexion fermée..\n\n");
                }
            }
            close(fdSocketCommunication);
            close(fdSocketAttente);
            return 0;
        }
    }
}