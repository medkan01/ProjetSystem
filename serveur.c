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
#define bool int //type booleen
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

Dossier liste[MAX_PLACES]; //creation d'une liste des dossiers, et donc du nombre de place dispo.
int nbDossier = 0; //variable qui donne le nombre de dossier créé

//declaration des fonctions
Dossier creationDossier(char nom[20], char prenom[20]);
void initSalle(Salle* s);
void afficherMenuPrincipal();
void procInscription(int socket);
void afficherMenuDesinscription();
int randint(int bi, int bs);
int numeroDossier();
void toString(int n, char str[]);

//procédure d'inscription d'un client
void procInscription(int socket){
    //declaration des variables
    char text[100], nom[30], prenom[30], noDoss[10];
    int nbRecu = 0;
    bool nomOk = false, prenomOk = false;
    Dossier d;
    //démarrage de la procédure d'inscription
    printf("Inscription du client:\n");
    //attente de réception du nom de la part du client
    while(nomOk == false){
        nbRecu = recv(socket, text, 99, 0);
        if(nbRecu > 0){
            text[nbRecu] = 0;
            printf("Nom: %s\n", text);
            *nom = *text;
            nomOk = true;
        }
    }
    //attente de réception du prénom de la part du client
    while(prenomOk == false){
        nbRecu = recv(socket, text, 99, 0);
        if(nbRecu > 0){
            text[nbRecu] = 0;
            printf("Prénom: %s\n", text);
            *prenom = *text;
            prenomOk = true;
        }
    }
    //remplissage du dossier
    d.noDossier = numeroDossier();
    *d.nom = *nom;
    *d.prenom = *prenom;
    //envoi du numero de dossier au client
    toString(d.noDossier, noDoss);
    strcpy(text, noDoss);
    send(socket, text, strlen(text), 0);
    //ajout du dossier à la liste + incrémentation du nombre de dossier actuel
    liste[nbDossier] = d;
    nbDossier++;
    //arret de la procédure d'inscription
    printf("Arret de la procédure d'inscription.\n\n");
}

//int to string
void toString(int n, char str[]){
    sprintf(str, "%i", n);
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
Dossier creationDossier(char nom[], char prenom[]){

}

//retourn un numero aleatoire
int randint(int bi, int bs){
    int n;
    n = rand() % (bs+1) + bi;
    return n;
}

//creer le numero de dossier dans une table d'entier
void createNoDossier(int n[10]){
    srand(time(null));
    int coef = 1;
    for(int i = 0; i < 10; i++){
        n[i] = randint(0, 9);
    }
}

//rempli une chaine de caractere avec les numeros du dossier pour faciliter l'envoie du numero au client
void noDossierToString(int noDossier[10]){
    for(int i = 0; i < 10; i++){
        printf("%i", noDossier[i]);
    }
    
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
                    ///////////////////////////////////////////
                    procInscription(fdSocketCommunication);
                    ///////////////////////////////////////////
                }
            }
            close(fdSocketCommunication);
            close(fdSocketAttente);
            return 0;
        }
    }
}