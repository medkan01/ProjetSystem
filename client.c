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
void procPlacesLibres(int socket);
bool verifPlaceDispo(int colonne, int range);
void reservationPlace(int colonne, int range);
int noPlace(int colonne, int range);

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

int noPlace(int colonne, int range){
    for(int i=0; i<100;i++){
        if((places[i].colonne == colonne) && (places[i].range == range)){
            return i;
        }
    }
}

void procInscription(int socket){
    //declaration des variables
    char nom[30], prenom[30], text[100], noDossier[TAILLE_NO_DOSSIER];
    bool dossierOk = false, placeOk = false;
    int nbRecu = 0, colonne, range, nPlace;
    //démarrage de la procédure d'inscription
    printf("Réservation de billet:\n");
    //saisie du nom du client
    printf("Veuillez saisir votre nom en majuscule:\n");
    scanf("%[^\n]", nom);
    getchar();
    //saisie du prenom du client
    printf("Veuillez saisir votre prénom:\n");
    scanf("%[^\n]", prenom);
    getchar();
    //attente de la reception de la liste des places disponible envoyé par le serveur
    send(socket, "1", 1, 0);
    printf("Si la liste des places disponibles ne s'affiche pas, c'est qu'un client est en train de choisir sa place.\nVeuillez patientez\n");
    recv(socket, places, sizeof(places), 0);
    while(!placeOk){
        //affichage des places
        afficherPlaces();
        //choix de la colonne
        printf("Veuillez choisir la colonne(X) de votre place:\n");
        scanf("%d", &colonne);
        getchar();
        //choix de la range
        printf("Veuillez choisir la range(Y) de votre place:\n");
        scanf("%d", &range);
        getchar();
        //vérification disponibilité de la place
        placeOk = verifPlaceDispo(colonne, range);
        if(!placeOk){
            printf("La place séléctionné n'est pas disponible veuillez en séléctionné une autre\n");
        }
    }
    //reservationPlace(colonne, range);
    nPlace = noPlace(colonne, range);
    send(socket, &nPlace, sizeof(nPlace), 0);
    strcpy(text, nom);
    send(socket, text, sizeof(text), 0);
    strcpy(text, prenom);
    send(socket, text, sizeof(text), 0);
    printf("no dossier = %s\n", noDossier);
    int n = recv(socket, noDossier, TAILLE_NO_DOSSIER, 0);
    noDossier[n] = 0;
    printf("%s\n", noDossier);
    printf("numéro de Dossier : %s\n", noDossier);
    printf("Attention ! Notez bien ce numéro de dossier.\nIl pourrait être demandé plus tard.\n\n");
    //arret de la procédure d'inscription
    printf("Arret de la procédure de réservation.\n\n");
}
/*
void procDesinscription(int socket){
    //declaration des variables
    char text[100], nom[30], noDossier[TAILLE_NO_DOSSIER];
    int nbRecu = 0;
    //démarrage de la procédure de désinscription
    printf("Annulation de réservation:\n");
    //saisie du nom
    printf("Veuillez saisir votre nom en majuscule:\n");
    scanf("%s", nom);
    getchar();
    //saisie du numéro de dossier
    printf("Veuillez saisir votre numéro de dossier:\n");
    scanf("%s", noDossier);
    getchar();
    //envoi du numéro de dossier au serveur
    send(socket, "2", 1, 0);
    //send(socket, &noDossier, sizeof(noDossier), 0);
    strcpy(text, nom);
    //send(socket, text, sizeof(text), 0);
    //suppression du dossier
    printf("En attente de suppression du dossier..\n");
    recv(socket, text, 99,0);
    printf("%s",text);
    //arret de la procédure de désinscription
    printf("Arret de la procédure d'annulation de dossier.\n\n");
}
*/

void procPlacesLibres(int socket){
    send(socket, "1", strlen("1"), 0);
    recv(socket, places, sizeof(places), 0);
    int n=0;//nombre de place dispo
    for(int i=0; i<100;i++){
        if(places[i].libre){n++;}
    }
    printf("Il y a %i place(s) disponible(s).\n\n\n",n);
    afficherPlaces();
}

bool verifPlaceDispo(int colonne, int range){
    for(int i=0; i<100;i++){
        if((places[i].libre) && (places[i].colonne == colonne) && (places[i].range == range)){
            return true;
        }
    }
    return false;
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
            printf("\n\nQue voulez vous faire:\n\n1.Réserver un billet\n2.Annuler une réservation\n3.Consulter le nombre de places disponibles\n");
            scanf("%i",choix);
            getchar();
            switch (*choix)
            {
            case 1:
                procInscription(fdSocket);
                break;
            case 2:
            /*
                procDesinscription(fdSocket);
                break;
            */
           case 3:
                procPlacesLibres(fdSocket);
                break;
            default:
                break;
            }
            close(fdSocket);
            
        }
    }
}