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
    char text[100], nom[30], prenom[30];
    bool dossierOk = false;
    int nbRecu = 0;
    //debut de la procédure d'inscription
    
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
            }
            close(fdSocket);
        }
    }
}