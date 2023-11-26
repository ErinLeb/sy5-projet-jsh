#include "lib/env.h"
#include "lib/prompt.h"
#include "lib/parseur.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
 
char * path_courant;
char * path_precedent;
int jobs;
int val_retour;
bool boucle;
int res; //pour tester les appels système


int main(int argc, char const *argv[]) {
    res = chdir(getenv("HOME"));
    if(res < 0) {
        perror("Erreur de déplacement dans HOME.\n");
        exit(EXIT_FAILURE);
    }
    path_courant = malloc(sizeof(char) * (PATH_MAX + 1));
    path_precedent = malloc(sizeof(char) * (PATH_MAX + 1));
    if(path_courant == NULL || path_precedent == NULL) {
        perror("Erreur d'allocation des chemins initiaux.\n");
        exit(EXIT_FAILURE);
    }
    path_courant = getcwd(path_courant, PATH_MAX + 1);
    path_precedent = getcwd(path_precedent, PATH_MAX + 1);
    if(path_courant == NULL) {
        perror("Erreur d'obtention des chemins initiaux.\n");
        exit(EXIT_FAILURE);
    }
    jobs = 0; 
    val_retour = 0; 
    boucle = true; 


    /* while(boucle){
        // on affiche l'invite de commande
        prompt(); 

        //On parse la ligne de commande rentrée  
        parseur();

    } */

    free(path_courant);
    free(path_precedent);
    return val_retour;
}