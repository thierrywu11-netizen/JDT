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

int ancienClassement[MAX_CONCOURS][MAX_PARTICIPANTS];
int ancienClassementInitialise[MAX_CONCOURS];

void inscrire(personne participants[], int *numParticipants) {
    personne p;
    scanf("%s %s", p.prenom, p.nom);

    for (int i = 0; i < *numParticipants; ++i) {     /*recherche si le participant existe déjà */
        if (strcmp(p.nom, participants[i].nom) == 0 && strcmp(p.prenom, participants[i].prenom) == 0) {
            printf("Nom incorrect\n");
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
        printf("Aucun participant inscrit\n");
        return;
    }

    for (int i = 0; i < *numParticipants; ++i) {
        int nbrCcours = 0;      /*compte le nombre de concours auxquels le participant a participé */
        for (int j = 0; j < *numConcours; ++j) {
            if (scores[i][j] > 0) {
                nbrCcours++;
            }
        }
        printf("(%d) %s %s : %d concours\n", i+1 , participants[i].prenom, participants[i].nom, nbrCcours);
    }

    return;
}

void creer(epreuve ccours[], int *numConcours) { // modifier les concours, y'en a trop pour différent de choses
    epreuve c;
    scanf("%s", c.jeu);

    for (int i = 0; i < *numConcours; ++i) {     // cherche si le concours existe déjà
        if (strcmp(c.jeu, ccours[i].jeu) == 0) {
            printf("Nom incorrect\n");
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
        int nbrParticpants = 0;      /*compte le nombre de participants qui participent à ce concours */
        for (int j = 0; j < *numParticipants; ++j) {
            if (scores[j][i] > 0) {
                nbrParticpants++;
            }
        }
        printf("(%d) %s : %d participant%s\n", i+1, ccours[i].jeu, nbrParticpants, nbrParticpants > 1 ? "s" : "");
   }

    return;
}

void gain(const personne participants[], const epreuve ccours[], const int *numParticipants, const int *numConcours, int scores[MAX_PARTICIPANTS][MAX_CONCOURS]) {
    int participantIndex, concoursIndex, points;
    scanf("%d %d %d", &participantIndex, &concoursIndex, &points);

    if (participantIndex < 1 || participantIndex > *numParticipants) {
        printf("Participant incorrect\n");
        return;
    }

    if (concoursIndex < 1 || concoursIndex > *numConcours) {
        printf("Concours incorrect\n");
        return;
    }

    if (points < 1) {
        printf("Points incorrects\n");
        return;
    }

    int indexConcours = concoursIndex - 1;
    if (!ancienClassementInitialise[indexConcours]) {
        for (int i = 0; i < *numParticipants; ++i) {
            ancienClassement[indexConcours][i] = 1;
        }
        ancienClassementInitialise[indexConcours] = 1;
    }

    for (int i = 0; i < points; ++i) {
        scores[participantIndex - 1][concoursIndex - 1]++;
    }

    printf("Gain enregistre\n");
}

void score(const personne participant[], const epreuve ccours[], const int *numParticipants, const int *numConcours, int scores[MAX_PARTICIPANTS][MAX_CONCOURS]) {
    int participantIndex;
    int gain = 0;
    scanf("%d", &participantIndex);

    if (participantIndex < 1 || participantIndex > *numParticipants) {
        printf("Identifiant incorrect\n"); 
        return;
    }

    printf("%s %s\n", participant[participantIndex - 1].prenom, participant[participantIndex - 1].nom);

    for (int i = 0; i < *numConcours; ++i) {
        if (scores[participantIndex - 1][i] > 0) {
            gain = 1;
            break;
        }
    }

    if (gain == 0) {
        printf("Aucun gain\n");
        return;
    }

    for (int i = 0; i < *numConcours; ++i) {
        if (scores[participantIndex - 1][i] > 0) {
            int points = scores[participantIndex - 1][i];
            printf("(%d) %s : %d point%s\n", i+1, ccours[i].jeu, points, points == 1 ? "" : "s");
        }
    }
}

void trie(int clssement[], int scores[MAX_PARTICIPANTS][MAX_CONCOURS], int numParticipants, int concoursIndex) {
    int temp;

    for (int i = 0; i < numParticipants; ++i) {
        clssement[i] = i;
    }

    for (int i = 0; i < numParticipants; ++i) {
        for (int j = 0; j + 1 < numParticipants - i; ++j) {
            if (scores[clssement[j]][concoursIndex] < scores[clssement[j + 1]][concoursIndex]) {
                temp = clssement[j];
                clssement[j] = clssement[j + 1];
                clssement[j + 1] = temp;
            }
        }
    }
}

void classement (const personne participants[], const epreuve ccours[], const int *numParticipants, const int *numConcours, int clssement[], int scores[MAX_PARTICIPANTS][MAX_CONCOURS]) {
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

    trie(clssement, scores, *numParticipants, concoursIndex - 1);
    int aScore = 0;
    for (int i = 0; i < *numParticipants; ++i) {
        if (scores[clssement[i]][concoursIndex - 1] > 0) {
            aScore++;
        }
    }

    if (aScore == 0) {
        printf("Aucun participant\n");
        return;
    }

    int rank;

    for (int i = 0; i < aScore; i++) {

        int points = scores[clssement[i]][concoursIndex - 1]; // Recupere les points du participant

        if (i == 0 || points != scores[clssement[i - 1]][concoursIndex - 1]) { 
            rank = i + 1;
        }

        if ((i > 0 && points == scores[clssement[i - 1]][concoursIndex - 1]) || (i < aScore - 1 && points == scores[clssement[i + 1]][concoursIndex - 1])) {
            printf("%dex -> (%d) %s %s : %d point%s\n", rank, clssement[i] + 1, participants[clssement[i]].prenom, participants[clssement[i]].nom, points, (points == 1) ? "" : "s");
        }
        else {
            printf("%d -> (%d) %s %s : %d point%s\n", rank, clssement[i] + 1, participants[clssement[i]].prenom, participants[clssement[i]].nom, points, (points == 1) ? "" : "s");
        }
    }
}

void comparaison(const personne participants[], const epreuve ccours[], const int *numParticipants, const int *numConcours, int scores[MAX_PARTICIPANTS][MAX_CONCOURS]) {
    int concoursIndex1, concoursIndex2;
    scanf("%d %d", &concoursIndex1, &concoursIndex2);

    if (concoursIndex1 < 1 || concoursIndex1 > *numConcours || concoursIndex2 < 1 || concoursIndex2 > *numConcours) {
        printf("Identifiant incorrect\n");
        return;
    }

    printf("%s versus %s\n", ccours[concoursIndex2 - 1].jeu, ccours[concoursIndex1 - 1].jeu);

    int siScores = 0;
    for (int i = 0; i < *numParticipants; ++i) {
        if (scores[i][concoursIndex1 - 1] > 0 || scores[i][concoursIndex2 - 1] > 0) {
            siScores = 1;
            break;
        }
    }

    if (siScores == 0) {
        printf("Aucun participant\n");
        return;
    }

    for (int i = 0; i < *numParticipants; ++i) {
        if (scores[i][concoursIndex2 - 1] > 0 && scores[i][concoursIndex1 - 1] == 0) {
            printf("Concours incompatibles\n");
            return;
        }
    }

    for (int i = 0; i < *numParticipants; ++i) {
        int points1 = scores[i][concoursIndex1 - 1];
        int points2 = scores[i][concoursIndex2 - 1];

        if (points1 == 0 && points2 == 0) {
            continue;
        }

        float ratio = (float)points2 / (float)points1;
        printf("(%d) %s %s : %d point%s/%d point%s : %.2f%%\n", i + 1, participants[i].prenom, participants[i].nom, points2, points2 <= 1 ? "" : "s", points1, points1 <= 1 ? "" : "s", ratio * 100);
    }
}

void bilan(const personne participants[], const epreuve ccours[], const int *numParticipants, const int *numConcours, int clssement[], int scores[MAX_PARTICIPANTS][MAX_CONCOURS]) {
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

    trie(clssement, scores, *numParticipants, concoursIndex - 1);
    int participantsAvecScore = 0;
    for (int i = 0; i < *numParticipants; ++i) {
        if (scores[clssement[i]][concoursIndex - 1] > 0) {
            participantsAvecScore++;
        }
    }

    if (participantsAvecScore == 0) {
        printf("Aucun participant\n");
        return;
    }

    int indexConcours = concoursIndex - 1;
    int rank;
    for (int i = 0; i < participantsAvecScore; ++i) {
        int p = clssement[i];
        int points = scores[p][indexConcours];

        if (i == 0 || points != scores[clssement[i - 1]][indexConcours]) {
            rank = i + 1;
        }

        int ancienRang = ancienClassement[indexConcours][p];
        if (ancienRang == 0) {
            ancienRang = 2;
        }
        int ecart = ancienRang - rank;
        const char *placeSuffix = (ecart < -1 || ecart > 1) ? "s" : "";
        const char *pointSuffix = points == 1 ? "" : "s";

        if ((i > 0 && points == scores[clssement[i - 1]][indexConcours]) ||
            (i < participantsAvecScore - 1 && points == scores[clssement[i + 1]][indexConcours])) {
            printf("%dex -> (%d) %s %s : %d point%s : %+d place%s\n",
                   rank, p + 1, participants[p].prenom, participants[p].nom,
                   points, pointSuffix, ecart, placeSuffix);
        }
        else {
            printf("%d -> (%d) %s %s : %d point%s : %+d place%s\n",
                   rank, p + 1, participants[p].prenom, participants[p].nom,
                   points, pointSuffix, ecart, placeSuffix);
        }
        ancienClassement[indexConcours][p] = rank;
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

    int clssement[MAX_PARTICIPANTS]; //liste des indices des participants triés par score descroissnate

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
            classement(participants, ccours, &numParticipants, &numConcours, clssement, scores);
        } // TODO
        
        else if (strcmp(cde, "COMPARAISON") == 0){
            comparaison(participants, ccours, &numParticipants, &numConcours, scores);
        } // TODO
        
        else if (strcmp(cde, "BILAN") == 0){
            bilan(participants, ccours, &numParticipants, &numConcours, clssement, scores);
        } // TODO
    } while (strcmp(cde, "EXIT") != 0);
}