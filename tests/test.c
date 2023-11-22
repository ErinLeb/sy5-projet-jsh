#include "../lib/env.h"
#include "../lib/pwd.h"
#include "../lib/last_output.h"
#include "../lib/exit.h"
#include "../lib/prompt.h"
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

DIR * courant; 
char * path_courant = "/";
int jobs = 0; 
int val_retour = 0;  
bool boucle = true;

void test_pwd(){
    printf("-------------------------TEST PWD----------------------------\n");
    path_courant = "path/du/répertoire/courant";
    pwd();

    path_courant = "nouveau/path/du/nouveau/répertoire/courant";
    pwd();
}

void test_last_output(){
    printf("-------------------------TEST ?----------------------------\n");
    val_retour = 1; 
    printf("Dernière valeur de retour attendue : 1 ; valeur obtenue : ");
    last_output();

    val_retour = 27;
    printf("Dernière valeur de retour attendue : 27 ; valeur obtenue : ");
    last_output();
}

void test_exit(){
        printf("-------------------------TEST EXIT----------------------------\n");
    printf("Dans la boucle : %d\n", boucle);
    jobs = 1;
    exit_jsh();
    printf("Exit avec jobs en cours : %d -- toujours dans la boucle ? %d\n", val_retour, boucle);

    jobs = 0;
    val_retour = 0;
    exit_jsh();
    printf("Exit (dernière valeur de retour 0) : %d -- toujours dans la boucle ? %d\n", val_retour, boucle);

    boucle = true;
    exit_jsh_with_arg(42);
    printf("Exit avec valeur 42 : %d -- toujours dans la boucle ? %d\n", val_retour, boucle);
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
    printf("\n\n");
    
    test_last_output();
    printf("\n\n");

    test_exit();
    printf("\n\n");
    
    test_prompt();
    printf("\n\n");


    if((closedir(courant) != 0)){
        write(STDERR_FILENO, "Erreur à la fermeture du répertoire courant.\n", 48);
        exit(EXIT_FAILURE);
    }
    return 0;
}