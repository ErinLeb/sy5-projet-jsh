#include "../lib/prompt.h"
#include "../lib/env.h"
#include "string.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


int min(int a, int b){
    if (a<b){
        return a;
    }
    return b;
}


char * prompt(char * prompt){
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
    path = malloc(sizeof(char) * (min(max_length, cur_length) + 1));
    if(path == NULL) {
        perror("Erreur d'allocation prompt\n");
        exit(EXIT_FAILURE);
    }
    
    if(max_length > cur_length) {
        memcpy(path, path_courant, cur_length);
        path[cur_length] = '\0';
    
    } else {
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

    sprintf(prompt,"\001\033[91m\002[%d]\001\033[32m\002%s\001\033[00m\002$ ",jobs, path);
    
    free(path);
    return prompt;
}