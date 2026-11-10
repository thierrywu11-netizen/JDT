#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void Exit() {
    exit(0);
}

enum {
    NB_CARACTERES = 31, MAX_PARTICIPANTS= 100, MAX_CONCOURS= 20
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

    for (int i = 0; i < *numParticipants; ++i) {     /*recherche si le participant existe déjà */
        if (strcmp(p.nom, participants[i].nom) == 0 && strcmp(p.prenom, participants[i].prenom) == 0) {
            printf("Nom Incorrect\n");
            return;
        }
    }

    participants[*numParticipants] = p;     /*ajout du participant */
    ++(*numParticipants);       /*identifiant du participant */
    printf("Inscription enregistree (%d)\n", *numParticipants);

    return;
}

void participant(const personne participants[], const int *numParticipants, const int *numConcours, const int scores[MAX_PARTICIPANTS][MAX_CONCOURS]) {

    if (*numParticipants == 0) {        /*verifie si y'a des participants */
        printf("Aucun participant\n");
        return;
    }

    for (int i = 0; i < *numParticipants; ++i) {
        int a = 0;      /*compte le nombre de concours auxquels le participant a participé */
        for (int j = 0; j < *numConcours; ++j) {
            if (scores[i][j] > 0) {
                a++;
            }
        }
        printf("(%d) %s %s : %d concours\n", i+1 , participants[i].prenom, participants[i].nom, a);
    }

    return;
}

void creer(epreuve ccours[], int *numConcours) { // modifier les concours, y'en a trop pour différent de choses
    epreuve c;
    scanf("%s", c.jeu);

    for (int i = 0; i < *numConcours; ++i) {     // cherche si le concours existe déjà
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

void concours(const epreuve ccours[], const int *numConcours, const int *numParticipants, const int scores[MAX_PARTICIPANTS][MAX_CONCOURS]) {
    
    if (*numConcours == 0) {
        printf("Aucun concours cree\n");
        return;
    }

    for (int i = 0; i < *numConcours; ++i) {
        int a = 0;      /*compte le nombre de participants qui participent à ce concours */
        for (int j = 0; j < *numParticipants; ++j) {
            if (scores[j][i] > 0) {
                a++;
            }
        }
        printf("(%d) %s : %d participant\n", i+1, ccours[i].jeu, a);
   }

    return;
}

void gain(const personne participants[], const epreuve ccours[], const int *numParticipants, const int *numConcours, int scores[MAX_PARTICIPANTS][MAX_CONCOURS]) {
    int participantIndex, concoursIndex, points;
    scanf("%d %d %d", &participantIndex, &concoursIndex, &points);

    if (participantIndex < 1 || participantIndex > *numParticipants || concoursIndex < 1 || concoursIndex > *numConcours) {
        printf("Index invalide\n");
        return;
    }

    for (int i = 0; i < points; ++i) {
        scores[participantIndex - 1][concoursIndex - 1]++;
    }

    printf("Gain enregistre\n");
}

void score(const personne participant[], const epreuve ccours[], const int *numParticipants, const int *numConcours, int scores[MAX_PARTICIPANTS][MAX_CONCOURS]) {
    int participantIndex;
    scanf("%d", &participantIndex);

    if (participantIndex < 1 || participantIndex > *numParticipants) {
        printf("Identifiant incorrect\n");
        return;
    }

    printf("%s %s:\n", participant[participantIndex - 1].prenom, participant[participantIndex - 1].nom);
    for (int i = 0; i < *numConcours; ++i) {
        if (scores[participantIndex - 1][i] > 0) {
            printf("(%d) %s: %d points\n", i+1, ccours[i].jeu, scores[participantIndex - 1][i]);
        }
    }
}

void classement (const personne participants[], const epreuve ccours[], const int *numParticipants, const int *numConcours, int scores[MAX_PARTICIPANTS][MAX_CONCOURS]) {
    int concoursIndex;
    scanf("%d", &concoursIndex);

    if (concoursIndex < 1 || concoursIndex > *numConcours) {
        printf("Identifiant incorrect\n");
        return;
    }
    printf("%s\n", ccours[concoursIndex - 1].jeu);
    
    if (*numParticipants == 0) {
        printf("Aucun participant\n");
        return;
    }

    int classement[MAX_PARTICIPANTS];   //liste des indices des participants triés par score descroissnate
    int temp;   //variable temporaire pour echanger les indices

    //initialisation du classement avec les indices des participants

    for (int i = 0; i < *numParticipants; ++i) {
        classement[i] = i;
    }

    /*faire le tri par score décroissant*/

    for (int i = 0; i < *numParticipants; ++i) {
        for (int j = i + 1; j < *numParticipants; ++j) {
            if (scores[classement[i]][concoursIndex - 1] < scores[classement[j]][concoursIndex - 1]) {
                temp = classement[i];
                classement[i] = classement[j];
                classement[j] = temp;
            }
        }
    }

    // affichage du classement

    int a;  //variable pour le classement

    for (int i = 0; i < *numParticipants; i++) {

        int points = scores[classement[i]][concoursIndex - 1]; // Recupere les points du participant

        if (i == 0 || points != scores[classement[i - 1]][concoursIndex - 1]) { 
            a = i + 1;
        }

        if ((i > 0 && points == scores[classement[i - 1]][concoursIndex - 1]) || (i < *numParticipants - 1 && points == scores[classement[i + 1]][concoursIndex - 1])) {
            printf("%dex -> (%d) %s %s : %d points\n", a, classement[i] + 1, participants[classement[i]].prenom, participants[classement[i]].nom, points);
        }
        else {
            printf("%d -> (%d) %s %s : %d points\n", a, classement[i] + 1, participants[classement[i]].prenom, participants[classement[i]].nom, points);
        }
    }
}


/*
ça sert pas à grand chose dans ton cas mais c'est bien de t'habituer à "return 0:" à la fin de ton main()
*/

int main() {
    personne participants[MAX_PARTICIPANTS];
    int numParticipants = 0;
    
    epreuve ccours[MAX_CONCOURS];
    int numConcours = 0;

    int scores[MAX_PARTICIPANTS][MAX_CONCOURS] = {0}; //faire le score avec ça, compter le nombre de non null pour avoir le nombre de concours auxquels le participant a participé, et pareil pour compter le nombre de participant

    char cde[31] = ""; // oups, ne serait-ce un nombre magique ?
    do {
        scanf("%s", cde);
        if (strcmp(cde, "INSCRIRE") == 0){
            inscrire(participants, &numParticipants);
        } // TODO

        else if (strcmp(cde, "PARTICIPANTS") == 0){
            participant(participants, &numParticipants, &numConcours, scores);
        } // TODO
        
        else if (strcmp(cde, "CREER") == 0){
            creer(ccours, &numConcours);
        } // TODO
        
        else if (strcmp(cde, "CONCOURS") == 0){
            concours(ccours, &numConcours, &numParticipants, scores);
        } // TODO
        
        else if (strcmp(cde, "GAIN") == 0){
            gain(participants, ccours, &numParticipants, &numConcours, scores); // &numparticipants et l'autre à comprendre pourquoi
        } // TODO
        
        else if (strcmp(cde, "SCORES") == 0){
            score(participants, ccours, &numParticipants, &numConcours, scores);
        } // TODO
        
        else if (strcmp(cde, "CLASSEMENT") == 0){
            classement(participants, ccours, &numParticipants, &numConcours, scores);
        } // TODO
        /*
        else if (strcmp(cde, "COMPARAISON") == 0){

        } // TODO
        else if (strcmp(cde, "BILAN") == 0){

        } // TODO */
    } while (strcmp(cde, "EXIT") != 0);
}
