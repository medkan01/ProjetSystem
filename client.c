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
#define LOCALHOST "127.0.0.1"
#define PORT 3000 //port de connexion
#define MAX_BUFFER 1000 //buffer
#define bool int //type booleen
#define true 1 //
#define false 0 //
#define null NULL //null
#define TAILLE_NO_DOSSIER 10 //taille du numero dossier

//declaration des fonctions
void procInscription(int socket);
void procDesinscription(int socket);

void procDesinscription(int socket){
    //declaration des variables
    char text[100], noDossier[TAILLE_NO_DOSSIER];
    int nbRecu = 0;
    bool procDesinscription = false;
    //démarrage de la procédure de désinscription
    printf("Désinscription:\n");
    //saisie du numéro de dossier
    printf("Veuillez saisir votre numéro de dossier:");
    scanf("%[^\n]", noDossier);
    getchar();
    //envoi du numéro de dossier au serveur
    strcpy(text, noDossier);
    send(socket, text, strlen(text), 0);
    //suppression du dossier
    printf("En attente de suppression du dossier..\n");
    while(procDesinscription == false){
        nbRecu = recv(socket, text, 99, 0);
        if(nbRecu > 0){
            text[nbRecu] = 0;
            printf("%s\n", text);
            procDesinscription = true;
        }
    }
    //arret de la procédure de désinscription
    printf("Arret de la procédure de désinscription.\n\n");
}

void procInscription(int socket){
    //declaration des variables
    char text[100], nom[30], prenom[30];
    bool dossierOk = false;
    int nbRecu = 0;
    //démarrage de la procédure d'inscription
    printf("Inscription:\n");
    //saisie du nom du client
    printf("Veuillez saisir votre nom:\n");
    scanf("%[^\n]", nom);
    getchar();
    //envoi du nom au serveur
    strcpy(text, nom);
    send(socket, text, strlen(text), 0);
    //saisie du prenom du client
    printf("Veuillez saisir votre prénom:\n");
    scanf("%[^\n]", prenom);
    getchar();
    //envoi du prénom au serveur
    strcpy(text, prenom);
    send(socket, text, strlen(text), 0);
    //attente de creation de la part du serveur
    printf("Veuillez patienter pendant la création du dossier..\n");
    while(dossierOk == false){
        nbRecu = recv(socket, text, 99, 0);
        if(nbRecu > 0){
            text[nbRecu] = 0;
            printf("Dossier numéro: %s\n\n\n", text);
            dossierOk = true;
        }
    }
    printf("Attention ! Notez bien ce numéro de dossier.\nIl pourrait être demandé plus tard.\n");
    //arret de la procédure d'inscription
    printf("Arret de la procédure d'inscription.\n\n");
}

int main(int argc, char const *argv[])
{
    //déclaration des variables
    int fdSocket;
    int nbRecu;
    struct sockaddr_in coordServeur;
    int longueurAdresse;
    char text[100];
    char adresseServeur[15];
    int choix[1];

    //initialisation socket
    fdSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(fdSocket < 0){
        printf("Erreur ! Socket incorrect\n");
        exit(EXIT_FAILURE);
    } else {
        //preparation coordServeur
        longueurAdresse = sizeof(struct sockaddr_in);
        memset(&coordServeur, 0x00, longueurAdresse);
        coordServeur.sin_family = PF_INET;
        //adresse serveur en local
        printf("Veuillez saisir l'adresse du serveur:\n");
        scanf("%[^\n]", adresseServeur);
        getchar();
        inet_aton(adresseServeur, &coordServeur.sin_addr);
        //Interfaces dispo
        coordServeur.sin_port = (htons(PORT));
        if(connect(fdSocket, (struct sockaddr *) &coordServeur, sizeof(coordServeur)) == -1){
            printf("Connexion impossible\n");
            exit(EXIT_FAILURE);
        } else {
            printf("Connexion réussie !\n");

            printf("Que voulez vous faire:\n\n1.Inscription\n2.Désinscription\n");
            scanf("%i",choix);
            getchar();
            switch (*choix)
            {
            case 1:
                strcpy(text, "1");
                send(fdSocket, text, strlen(text), 0);
                procInscription(fdSocket);
                break;
            case 2:
                strcpy(text, "2");
                send(fdSocket, text, strlen(text), 0);
                procDesinscription(fdSocket);
                break;
            default:
                break;
            }
            close(fdSocket);
        }
    }
}