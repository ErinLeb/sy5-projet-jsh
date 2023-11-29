#include "../lib/cd.h"
#include "../lib/env.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>


int cd(){
    int res = chdir(getenv("HOME"));
    
    if(res < 0) {
        perror("Erreur de déplacement dans cd.\n");
        return 1;
    }

    path_precedent = memcpy(path_precedent, path_courant, PATH_MAX + 1);
    path_courant = getcwd(path_courant, PATH_MAX + 1);
    
    if(path_courant == NULL) {
        perror("Erreur de mise à jour du chemin courant dans cd.\n");
        return 1;
    }

    return 0;
}


int cd_with_arg(char* arg){
    int res;
    
    if(strcmp("-", arg) == 0) {
        res = chdir(path_precedent);
    }
    else {
        res = chdir(arg);
    }

    if(res < 0) {
        perror("Erreur de déplacement dans cd_with_arg.\n");
        return 1;
    }

    path_precedent = memcpy(path_precedent, path_courant, PATH_MAX + 1);
    path_courant = getcwd(path_courant, PATH_MAX + 1);
    
    if(path_courant == NULL || path_precedent == NULL) {
        perror("Erreur de mise à jour du chemin courant ou précédent dans cd_with_args.\n");
        return 1;
    }

    return 0;
}