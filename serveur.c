#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 1000 //port de connexion
#define MAX_BUFFER 1000 //buffer
#define boolean int //type booleen
#define true 1 //
#define false 0 //
#define MAX_PLACES 100 //nombre maximum de places

typedef struct{
    boolean instanceOf; //boolean permettant de savoir si la structure actuelle est utilisée ou non

    int places_occupees; //nombre de places occupées
    int places_libres; //nombre de places libres dans la salle
} salle;

typedef struct{
    boolean instanceOf; //boolean permettant de savoir si la structure actuelle est utilisée ou non

    int no_dossier; //numéro du dossier
    char nom[100]; //nom de la personne associée au numéro de dossier
} dossier;