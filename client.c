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
#define boolean int //type booleen
#define true 1 //
#define false 0 //
#define null NULL //null

//declaration des fonctions
void procInscription(int fdSocket);

void procInscription(int fdSocket){
    //declaration des variables
    char text[100];
    int nbRecu;
    char nom[20], prenom[20];

    //reception de la procedure d'inscription de la part du serveur --nom
    nbRecu = recv(fdSocket, text, 99, 0);
    if(nbRecu > 0){
        text[nbRecu] = 0;
        printf("Reçu: %s\n", text);
        //saisie puis envoi du nom
        scanf("%[^\n]", nom);
        getchar();
        send(fdSocket, nom, strlen(text), 0);
        //reception de la procedure d'inscription de la part du serveur --prenom
        nbRecu = recv(fdSocket, text, 99, 0);
        if(nbRecu > 0){
            text[nbRecu] = 0;
            printf("Reçu: %s\n", text);
            //saisie puis envoi du prenom
            scanf("%[^\n]", prenom);
            getchar();
            send(fdSocket, prenom, strlen(text), 0);
            //reception de la procedure d'inscription de la part du serveur --noDossier
            nbRecu = recv(fdSocket, text, 99, 0);
            if(nbRecu > 0){
                text[nbRecu] = 0;
                printf("Reçu: %s\n", text);
            }
        }
    } else {
        printf("Erreur");
    }
}

int main(int argc, char const *argv[])
{
    //déclaration des variables
    int fdSocket;
    int nbRecu;
    struct sockaddr_in coordServeur;
    int longueurAdresse;
    char message[100];
    char adresseServeur[15];

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
            while(true){
                nbRecu = recv(fdSocket, message, 99, 0);
                if(nbRecu > 0){
                    message[nbRecu] = 0;
                    printf("Reçu: %s\n", message);
                }
                ////////////////////////////////////////////////////////
                if(message == "inscription"){
                    procInscription(fdSocket);
                }
                ////////////////////////////////////////////////////////
                send(fdSocket, message, strlen(message), 0);

            }
            close(fdSocket);
        }
    }
}