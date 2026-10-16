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
} Personne;

/*
typedef struct {
    char prenom[NB_CARACTERES];
    char nom[NB_CARACTERES];
} Personne;

void modifier_personne(Personne *personne_a_modifier) {
scan f f prénom nom
personn_a_modifier->prenom = prenom;

}

*/


int Inscrire(Personne participants[], int *numParticipants) {
    Personne p;
    scanf("%s %s", p.prenom, p.nom);

    for (int i = 0; i < *numParticipants; ++i) {
        if (strcmp(p.nom, participants[i].nom) == 0 && strcmp(p.prenom, participants[i].prenom) == 0) {
            printf("Nom Incorrect\n");
            return 0;
        }
    }

    participants[*numParticipants] = p;
    ++(*numParticipants);
    printf("Inscription enregistree (%d)\n", *numParticipants);

    return 0;
}

// il le fait à l'infini, comprends pas


int Participants(const Personne participants[], const int *numParticipants) {
    if (*numParticipants == 0) {
        printf("Aucun participant\n");
        return 0;
    }

    else {
        for (int i = 0; i < *numParticipants; ++i) {
            printf("(%d) %s %s : 0 concours\n", i+1 , participants[i].prenom, participants[i].nom);
        }
    }
    return 0;
}



int main() {
    Personne participants[MAX_PARTICIPANTS];
    int numParticipants = 0;
    
    char cde[31] = ""; // oups, ne serait-ce un nombre magique ?
    do {
        scanf("%s", cde);
        if (strcmp(cde, "INSCRIRE") == 0){
            Inscrire(participants, &numParticipants);
        } // TODO
        else if (strcmp(cde, "PARTICIPANTS") == 0){
             Participants(participants, &numParticipants);
             return 0;
        } // TODO
        /*
        else if (strcmp(cde, "CREER") == 0){

        } // TODO
        else if (strcmp(cde, "CONCOURS") == 0){

        } // TODO
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