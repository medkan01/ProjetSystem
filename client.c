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

typedef struct{
    bool libre;
    int range;
    int colonne;
} Place;
Place places[100];

//declaration des fonctions
void afficherPlaces();
void procInscription(int socket);
void procDesinscription(int socket);

void afficherPlaces(){
    int range=1;
    printf(" ");
    for(int colonne=1;colonne<11;colonne++){printf("  %i",colonne);}printf(" <-X");
    for(int i=0; i<100;i++){
       
        if(i%10==0){if(range!=10){printf("\n %i",range);range++;}else{printf("\n10");}}
        if(places[i].libre){
            printf(" O ");
        } else {
            printf(" X ");}
        
    }
    printf("\nÎ\nY\n");

}

void procInscription(int socket){
    //declaration des variables
    char text[100], nom[30], prenom[30];
    int colonne, range;
    bool dossierOk = false;
    int nbRecu = 0;
    //démarrage de la procédure d'inscription
    printf("Réservation de billet:\n");
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
    //choix de la place du client
    recv(socket, places, sizeof(places), 0);
    afficherPlaces();
    printf("Veuillez choisir la colonne(X) de votre place:\n");
    scanf("%[^\n]", colonne);
    getchar();

    printf("Veuillez choisir la range(Y) de votre place:\n");
    scanf("%[^\n]", range);
    getchar();

    //envoi de la place
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
        } else {
            printf("Erreur");
            exit(EXIT_FAILURE);
        }
    }
    printf("Attention ! Notez bien ce numéro de dossier.\nIl pourrait être demandé plus tard.\n\n");
    //arret de la procédure d'inscription
    printf("Arret de la procédure de réservation.\n\n");
}

void procDesinscription(int socket){
    //declaration des variables
    char text[100], noDossier[TAILLE_NO_DOSSIER];
    int nbRecu = 0;
    bool procDesinscription = false;
    //démarrage de la procédure de désinscription
    printf("Annulation de réservation:\n");
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
    printf("Arret de la procédure d'annulation de dossier.\n\n");
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

            printf("Que voulez vous faire:\n\n1.Réserver un billet\n2.Annuler une réservation\n3.Consulter le nombre de places disponibles\n");
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
            /*case 3:
                strcpy(text, "3");
                send(fdSocket, text, strlen(text), 0);
                procPlacesLibres(fdSocket);
                break;*/
            default:
                break;
            }
            close(fdSocket);
        }
    }
}