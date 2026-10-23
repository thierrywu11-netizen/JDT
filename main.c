#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Exit() {
    exit(0);
}

enum {
    NB_CARACTERES = 30, MAX_PARTICIPANTS= 100
}; 

typedef struct {
    char prenom[NB_CARACTERES];
    char nom[NB_CARACTERES];
} personne;

typedef struct {
    char jeu[NB_CARACTERES];
} epreuve;

void inscrire(personne participants[], int *numParticipants) {
    personne p;
    scanf("%s %s", p.prenom, p.nom);

    for (int i = 0; i < *numParticipants; ++i) {
        if (strcmp(p.nom, participants[i].nom) == 0 && strcmp(p.prenom, participants[i].prenom) == 0) {
            printf("Nom Incorrect\n");
            return;
        }
    }

    participants[*numParticipants] = p;
    ++(*numParticipants);
    printf("Inscription enregistree (%d)\n", *numParticipants);

    return;
}

void participant(const personne participants[], const int *numParticipants, int *nbrConcours) {
    *nbrConcours = 0;

    if (*numParticipants == 0) {
        printf("Aucun participant\n");
        return;
    }

    for (int i = 0; i < *numParticipants; ++i) {
        printf("(%d) %s %s : %d concours\n", i+1 , participants[i].prenom, participants[i].nom, *nbrConcours);
    }
    return;
}

void creer(epreuve ccours[], int *numConcours) { // modifier les concours, y'en a trop pour différent de choses
    epreuve c;
    scanf("%s", c.jeu);

    for (int i = 0; i < *numConcours; ++i) {  // faire une focntion à part pour comparer (INSCRCIRE)
        if (strcmp(c.jeu, ccours[i].jeu) == 0) {
            printf("Nom Incorrect\n");
            return;
        }
    }

    ccours[*numConcours] = c;
    ++(*numConcours);
    printf("Creation enregistree (%d)\n", *numConcours);

    return;
}

void concours(const epreuve ccours[], const int *numConcours, int *nbrParticipants) {
    *nbrParticipants = 0;
    if (*numConcours == 0) {
        printf("Aucun concours cree\n");
        return;
    }

    for (int i = 0; i < *numConcours; ++i) {
        printf("(%d) %s : %d participant\n", i+1, ccours[i].jeu, *nbrParticipants);
    }

    return;
}

void Gain(const personne participants[], const epreuve ccours[], int *nbrConcours, int *nbrParticipants) {
    int participantIndex, concoursIndex;
    scanf("%d %d", &participantIndex, &concoursIndex);
    
    if (participantIndex < 1 || participantIndex > MAX_PARTICIPANTS || concoursIndex < 1 || concoursIndex > MAX_PARTICIPANTS) {
        printf("Index invalide\n");
        return;
    }

    *nbrConcours +=1;
    *nbrParticipants +=1;

    return;
} // participants ne marche pas une deuxième fois

/*
ça sert pas à grand chose dans ton cas mais c'est bien de t'habituer à "return 0:" à la fin de ton main()
*/

int main() {
    personne participants[MAX_PARTICIPANTS];
    int numParticipants = 0;
    
    epreuve ccours[MAX_PARTICIPANTS];
    int numConcours = 0;

    char cde[31] = ""; // oups, ne serait-ce un nombre magique ?
    do {
        scanf("%s", cde);
        if (strcmp(cde, "INSCRIRE") == 0){
            inscrire(participants, &numParticipants);
        } // TODO

        else if (strcmp(cde, "PARTICIPANTS") == 0){
            participant(participants, &numParticipants, &numConcours);
        } // TODO
        
        else if (strcmp(cde, "CREER") == 0){
            creer(ccours, &numConcours);
        } // TODO
        
        else if (strcmp(cde, "CONCOURS") == 0){
            concours(ccours, &numConcours, &numParticipants);
        } // TODO
        /*
        else if (strcmp(cde, "GAIN") == 0){

        } // TODO
        else if (strcmp(cde, "SCORES") == 0){

        } // TODO
        else if (strcmp(cde, "CLASSEMENT") == 0){

        } // TODO
        else if (strcmp(cde, "COMPARAISON") == 0){

        } // TODO
        else if (strcmp(cde, "BILAN") == 0){

        } // TODO */
    } while (strcmp(cde, "EXIT") != 0);
}
