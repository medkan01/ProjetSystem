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
#define TAILLE_NAME 15

//delcaration des structures
typedef struct{
    int placesLibres; //nombre de places libres dans la salle
} Salle;

typedef struct{
    char nom[TAILLE_NAME]; //nom de la personne associée au numéro de dossier
    char prenom[TAILLE_NAME]; //prenom de la personne associé au numéro de dossier
    char noDossier[TAILLE_NO_DOSSIER]; //numéro du dossier
    char bufferReset[100]; //cette variable permet au buffer de se reset car sinon le nom du dossier [n+1] se concatene au numéro de dossier [n]
} Dossier;

Dossier liste[MAX_PLACES]; //creation d'une liste des dossiers, et donc du nombre de place dispo.
int nbDossierTotal = 0; //variable qui donne le nombre de dossier créé

//declaration des fonctions
int randint(int bi, int bs); //fini
void noDossierToString(int noDossier[TAILLE_NO_DOSSIER], char str[TAILLE_NO_DOSSIER]); //fini
void createNoDossier(char noDossier[TAILLE_NO_DOSSIER]); //fini
void toString(int n, char str[]); //fini
void procInscription(int socket); //fini
void afficheDossiers(); //fini
void procDesinscription(int socket);
int rechercheDossier(char noDossier[TAILLE_NO_DOSSIER]);
void supprimerDossier(int emplacement);

//procédure d'inscription d'un client
void procInscription(int socket){
    //declaration des variables
    char text[100], nom[TAILLE_NAME], prenom[TAILLE_NAME], str[TAILLE_NO_DOSSIER];
    int nbRecu = 0;
    bool nomOk = false, prenomOk = false;
    Dossier d;
    srand(time(null));
    //démarrage de la procédure d'inscription
    printf("Inscription du client:\n");
    //creation du numero de dossier
    createNoDossier(d.noDossier);
    //attente de réception du nom de la part du client
    printf("En attente de la réception du nom de la part du client..\n");
    while(nomOk == false){
        nbRecu = recv(socket, text, 99, 0);
        if(nbRecu > 0){
            text[nbRecu] = 0;
            printf("Nom: %s\n", text);
            strcpy(d.nom, text);
            nomOk = true;
        }
    }
    //attente de réception du prénom de la part du client
    printf("En attente de la réception du prénom de la part du client..\n");
    while(prenomOk == false){
        nbRecu = recv(socket, text, 99, 0);
        if(nbRecu > 0){
            text[nbRecu] = 0;
            printf("Prénom: %s\n", text);
            strcpy(d.prenom, text);
            prenomOk = true;
        }
    }
    //etape de creation du dossier
    printf("Création du dossier en cours..\n");
    //envoi du numero de dossier au client
    strcpy(text, d.noDossier);
    send(socket, text, strlen(text), 0);
    //ajout du dossier à la liste
    liste[nbDossierTotal] = d;
    nbDossierTotal++;
    //arret de la procédure d'inscription
    printf("Arret de la procédure d'inscription.\n\n");
}

//retourne un numero aleatoire
int randint(int bi, int bs){
    int n;
    n = rand() % (bs+1) + bi;
    return n;
}

//creer le numero de dossier dans une table d'entier
void createNoDossier(char noDossier[TAILLE_NO_DOSSIER]){
    int n[TAILLE_NO_DOSSIER];
    for(int i = 0; i < TAILLE_NO_DOSSIER; i++){
        n[i] = randint(0, 9);
    }
    noDossierToString(n, noDossier);
}

//rempli une chaine de caractere avec les numeros du dossier pour faciliter l'envoie du numero au client
void noDossierToString(int noDossier[TAILLE_NO_DOSSIER], char str[TAILLE_NO_DOSSIER]){
    char chiffre[1];
    for(int i = 0; i < TAILLE_NO_DOSSIER; i++){
        toString(noDossier[i], chiffre);
        strcat(str, chiffre);
    }
}

//int to string
void toString(int n, char str[]){
    sprintf(str, "%i", n);
}

void afficheDossiers(){
    char str[TAILLE_NO_DOSSIER];
    printf("Liste des dossiers actuels:\n");
    for(int i = 0; i < nbDossierTotal; i++){
        printf("\nDossier: %i\n\n", i+1);
        printf("No Dossier: %s\n", liste[i].noDossier);
        printf("Nom: %s\n", liste[i].nom);
        printf("Prénom: %s\n", liste[i].prenom);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

//procédure de désinscription d'un client
void procDesinscription(int socket){
    //déclaration des variables
    char text[100], noDossier[TAILLE_NO_DOSSIER];
    int nbRecu = 0;
    bool noDossierOk = false;
    int emplacementDossier = -1;
    //démarrage de la procédure de désinscription
    printf("Désinscription:\n");
    //Attente de réception du numéro de dossier de la part du client
    printf("En attente de la réception du numéro de dossier du client..\n");
    while(noDossierOk == false){
        nbRecu = recv(socket, text, 99, 0);
        if(nbRecu > 0){
            text[nbRecu] = 0;
            printf("No Dossier: %s\n", text);
            strcpy(noDossier, text);
            noDossierOk = true;
        }
    }
    //recherche du dossier à l'aide du numéro de dossier saisi
    emplacementDossier = rechercheDossier(noDossier);
    if(emplacementDossier == -1){
        //dossier inexistant avec le numéro saisi. La procédure sera donc stoppée.
        printf("Le numéro de dossier est introuvable.\n");
    } else {
        //suppression du dossier car celui-ci est existant
        supprimerDossier(emplacementDossier);
        printf("Le dossier à été supprimé avec succés !\n");
    }
    //Arret de la procédure de désinscription
    printf("Arret de la procédure de désincription.\n\n");
}

//recherche un dossier grace au numéro, retourne l'emplacement dans la table s'il est trouvé, sinon retourne -1
int rechercheDossier(char noDossier[TAILLE_NO_DOSSIER]){
    Dossier d;
    for(int i = 0; i < nbDossierTotal; i++){
        if(noDossier == liste[i].noDossier){
            return i;
        }
    }
    return -1;
}

void supprimerDossier(int emplacement){
    for(int i = emplacement; i < nbDossierTotal; i++){
        if(nbDossierTotal < 1){
            if(i != nbDossierTotal-1){
                liste[i] = liste[i+1];
            }
        }
    }
    nbDossierTotal--;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    char text[100];
    char inscription[100] = "1", desinscription[100] = "2";
    int choix[1];

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
                    nbRecu = recv(fdSocketCommunication, choix, 99, 0);
                    if(nbRecu > 0){
                        choix[nbRecu] = 0;
                        if(*choix == '1'){
                            procInscription(fdSocketCommunication);
                            afficheDossiers();
                        } else if(*choix == '2'){
                            procDesinscription(fdSocketCommunication);
                        } else {
                            printf("Erreur");
                            exit(EXIT_FAILURE);
                        }
                    }
                }
            }
            close(fdSocketCommunication);
            close(fdSocketAttente);
            return 0;
        }
    }
}