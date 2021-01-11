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
    bool libre;
    int range;
    int colonne;
} Place;

typedef struct{
    char nom[TAILLE_NAME]; //nom de la personne associée au numéro de dossier
    char prenom[TAILLE_NAME]; //prenom de la personne associé au numéro de dossier
    char noDossier[TAILLE_NO_DOSSIER]; //numéro du dossier
    char bufferReset[100];
    Place place;
} Dossier;

Place places[MAX_PLACES];//table contenant toutes les places de la salle avec les attributs de chacune.
Dossier liste[MAX_PLACES]; //creation d'une liste des dossiers, et donc du nombre de place dispo.
int nbDossierTotal = 0; //variable qui donne le nombre de dossier créé

//declaration des fonctions
int randint(int bi, int bs); //fini
void noDossierToString(int noDossier[TAILLE_NO_DOSSIER], char str[TAILLE_NO_DOSSIER]); //fini
void createNoDossier(char noDossier[TAILLE_NO_DOSSIER]); //fini
void toString(int n, char str[]); //fini
void afficheDossiers(); 
int rechercheDossier(char noDossier[TAILLE_NO_DOSSIER]);
void procDesinscription(int socket);
void supprimerDossier(int emplacement);
void procPlacesLibres(int socket);
void afficherPlaces();
void reservePlace(int nPlace);
void ajoutDossier(Dossier d);
void afficherDossier();
void remplissageTablePlaces();
int noPlace(int colonne, int range);
int noPlace(int colonne, int range);

void procInscription(int socket){
    //déclaration des variables
    int nPlace, nbRecu;
    char prenom[TAILLE_NAME], nom[TAILLE_NAME], text[100], noDossier[TAILLE_NO_DOSSIER];
    Dossier d;
    srand(time(null));
    //envoi de la liste des places
    send(socket, places, sizeof(places), 0);
    //recois la liste des places
    nbRecu = recv(socket, &nPlace, sizeof(nPlace), 0);
    //recois le nom et le prenom
    nbRecu = recv(socket, nom, sizeof(nom), 0);
    nom[nbRecu] = 0;
    nbRecu = recv(socket, prenom, sizeof(prenom), 0);
    prenom[nbRecu] = 0;
    //reserve la place que le client à choisis
    reservePlace(nPlace);
    //créer le dossier du client
    createNoDossier(noDossier);
    send(socket, noDossier, sizeof(noDossier), 0);
    strcpy(d.nom, nom);
    strcpy(d.prenom, prenom);
    strcpy(d.noDossier, noDossier);
    d.place = places[nPlace];
    ajoutDossier(d);
    send(socket, places, sizeof(places), 0);
}

//procédure de désinscription d'un client
void procDesinscription(int socket){
    //déclaration des variables
    char text[100], nom[TAILLE_NAME], noDossier[TAILLE_NO_DOSSIER];
    int emplacementDossier, nbRecu;
    //reception du numéro de dossier
    nbRecu = recv(socket, noDossier, sizeof(noDossier), 0);
    noDossier[nbRecu] = 0;
    emplacementDossier = rechercheDossier(noDossier);
    if(emplacementDossier == -1){
        strcpy(text, "Le nom ou le numéro de dossier est incorect.\n");
        send(socket, text, sizeof(text), 0);
    } else {
        supprimerDossier(emplacementDossier);
        strcpy(text, "La réservation du dossier à été annulé.\n");
        send(socket, text, sizeof(text), 0);
    }
    send(socket, places, sizeof(places), 0);
}

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

//affiche les dossiers actuellement dans la liste
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

void remplissageTablePlaces(){
    int range=0;
    for(int i=0; i<MAX_PLACES;i++){
        places[i].colonne=i%10+1;
        if(i%10==0){
            range++;
            places[i].range=range;
        }else{places[i].range=range;}
        
        places[i].libre=true;
    }
}

void reservePlace(int nPlace){
    places[nPlace].libre=false;
}

//recherche un dossier grace au numéro, retourne l'emplacement dans la table s'il est trouvé, sinon retourne -1
int rechercheDossier(char noDossier[TAILLE_NO_DOSSIER]){
    int emplacement = -1;
    int test;
    for(int i = 0; i < nbDossierTotal; i++){
        printf("[debug] : 195 -- noDossier = [%s] - liste[i].noDossier = [%s]\n", noDossier, liste[i].noDossier);
        test = strcmp(noDossier, liste[i].noDossier);
        if(test == 0){
            emplacement = i;
            printf("[debug] : 198");
        }
    }
    return emplacement;
}

//supprime le dossier à l'emplacement entré en paramètre
void supprimerDossier(int emplacement){
    places[noPlace(liste[emplacement].place.colonne,liste[emplacement].place.range)].libre=true;
    for(int i = emplacement; i < nbDossierTotal-1; i++){
        liste[i] = liste[i+1];
    }
    nbDossierTotal--;

}

int noPlace(int colonne, int range){
    for(int i=0; i<100;i++){
        if((places[i].colonne == colonne) && (places[i].range == range)){
            return i;
        }
    }
}

void ajoutDossier(Dossier d){
    liste[nbDossierTotal] = d;
    nbDossierTotal++;
}

//envoi le nombre de places libres au client
void procPlacesLibres(int socket){
    send(socket, places, sizeof(places), 0);
}

//main program
int main(int argc, char const *argv[])
{
    //déclaration des variables
    remplissageTablePlaces();
    int fdSocketAttente;
    int fdSocketCommunication;
    struct sockaddr_in coordServeur;
    struct sockaddr_in coordClient;
    int longueurAdresse;
    int nbRecu;
    int noDossier; // numero de dossier du client qui veux se desinscrire
    int nPlace; //numero de la place reserver par le client
    Dossier d;
    char nomCl, prenomCl;
    char text[100];
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
                        } else if(*choix == '2'){
                            procDesinscription(fdSocketCommunication);
                        } else if(*choix == '3'){
                            procPlacesLibres(fdSocketCommunication);
                        } else {
                            printf("Erreur d'interprétation de demande\n");
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