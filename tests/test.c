#include "../lib/env.h"
#include "../lib/pwd.h"
#include "../lib/prompt.h"
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

DIR * courant; 
char * path_courant = "/";
int jobs = 0; 
int ret = 0;  
//bool boucle = true;

void test_pwd(){
    printf("-------------------------TEST PWD----------------------------\n");
    path_courant = "path/du/répertoire/courant";
    pwd();

    path_courant = "nouveau/path/du/nouveau/répertoire/courant";
    pwd();
}

void test_prompt() {
    printf("-------------------------TEST PROMPT----------------------------\n");
    jobs = 0;
    path_courant = "/";
    prompt();
    printf("\n");
    path_courant = "/court";
    prompt();
    printf("\n");
    path_courant = "/trop/long/pour/etre/affiche/correctement";
    prompt();
    printf("\n");
}

int main(){
    courant = opendir("/");
    if(courant == NULL){
        write(STDERR_FILENO, "Problème à l'ouverture du répertoire initial.\n", 50);
        exit(EXIT_FAILURE);
    }

    test_pwd();
    test_prompt();

    if((closedir(courant) != 0)){
        write(STDERR_FILENO, "Erreur à la fermeture du répertoire courant.\n", 48);
        exit(EXIT_FAILURE);
    }
    return 0;
}