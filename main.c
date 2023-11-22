#include "lib/env.h"
#include "lib/prompt.h"
#include "lib/parseur.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>


DIR * courant; 
char * path_courant;
int jobs;
int val_retour;
bool boucle;

int res; //pour tester les appels système


int main(int argc, char const *argv[]){
    path_courant = "/";
    jobs = 0; 
    val_retour = 0;  
    boucle = true; 

    courant = opendir("/");
    if(courant == NULL){
        perror("Problème à l'ouverture du répertoire initial.\n");
        exit(EXIT_FAILURE);
    }


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
    return val_retour;
}