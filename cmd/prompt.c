#include "../lib/prompt.h"
#include "../lib/env.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>

#define fmin( x, y )

/**
 * affiche un prompt de longeur maximale trente caractères en tronquant éventuellement le chemin vers le répertoire courant
*/
void prompt(){
    char *path;
    int max_length = 26; // longueur maximale du chemin à laquelle on enlève déjà les caractères invariants
    int nb_jobs = jobs;
    int jobs_length = 1;
    int cur_length = strlen(path_courant);

    while(nb_jobs / 10 != 0) {
        ++jobs_length;
        nb_jobs /= 10;
    }

    max_length -= jobs_length;
    path = malloc(sizeof(char) * (fmin(max_length, cur_length) + 1));
    if(path == NULL) {
        perror("Erreur d'allocation prompt");
        exit(EXIT_FAILURE);
    }
    
    if(max_length > strlen(path_courant)) {
        memcpy(path, path_courant, cur_length);
        path[cur_length] = '\0';
    }
    else {
        for(int i = 0; i < max_length; ++i) {
            if(i < 3) {
                path[i] = '.';
            }
            else {
                path[i] = path_courant[cur_length - max_length + i];
            }
        }
        path[max_length] = '\0';
    }

    fprintf(stderr, "\001\033[91m\002[%d]", jobs);
    fprintf(stderr, "\001\033[32m\002%s", path);
    fprintf(stderr, "\001\033[00m\002 $");
    
    free(path);
}