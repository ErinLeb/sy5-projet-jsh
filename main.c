#include "lib/env.h"
#include "lib/prompt.h"
#include "lib/parseur.h"
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>

DIR * courant; 
char * path_courant;
int jobs;
int ret; 
bool boucle;


int res;


int main(int argc, char const *argv[]){
    courant = opendir("/");
    if(courant == NULL){
        perror("Problème à l'ouverture du répertoire initial.\n");
        exit(EXIT_FAILURE);
    }
    path_courant = "/";
    jobs = 0; 
    ret = 0; 
    boucle = true; 


    /* while(boucle){
        // on affiche l'invite de commande
        prompt(); 

        //On parse la ligne de commande rentrée  
        parseur();

    } */


    res = closedir(courant);
    if(res != 0){
        perror("Erreur à la fermeture du répertoire courant.\n");
        exit(EXIT_FAILURE);
    }
    return ret;
}
