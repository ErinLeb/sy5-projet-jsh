#include "lib/env.h"
#include "lib/prompt.h"
#include "lib/parseur.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <string.h>
#include <signal.h>

int default_fd [3];
char * path_courant;
char * path_precedent;
int cmp_jobs;
job * pid_jobs[NBR_MAX_JOBS];
bool id_taken[NBR_MAX_JOBS];
int val_retour;
bool boucle;

int res; //pour tester les appels système


int main(int argc, char const *argv[]) {
    // initialisation des variables
    res = chdir(".");
    if(res < 0) {
        perror("Erreur d'initialisation du répertoire courant.");
        exit(EXIT_FAILURE);
    }

    default_fd[0] = dup(STDIN_FILENO);
    default_fd[1] = dup(STDOUT_FILENO);
    default_fd[2] = dup(STDERR_FILENO);


    path_courant = malloc(sizeof(char) * (PATH_MAX + 1));
    path_precedent = malloc(sizeof(char) * (PATH_MAX + 1));
    if(path_courant == NULL || path_precedent == NULL) {
        perror("Erreur d'allocation des chemins initiaux.");
        exit(EXIT_FAILURE);
    }

    path_courant = getcwd(path_courant, PATH_MAX + 1);
    path_precedent = getcwd(path_precedent, PATH_MAX + 1);
    if(path_courant == NULL) {
        perror("Erreur d'obtention des chemins initiaux.");
        exit(EXIT_FAILURE);
    }

    cmp_jobs = 0;
    val_retour = 0; 
    boucle = true; 

    char* ligne_cmd = NULL;
    char* prompt_char = malloc(sizeof(char) * PROMPT);

    rl_outstream = stderr;
    

    // boucle principale
    while(boucle){
        prompt_char = prompt(prompt_char); 
         
        ligne_cmd = readline(prompt_char);
        add_history(ligne_cmd);
        parseur_redirections(ligne_cmd);
        free(ligne_cmd);
        check_jobs_info ();
    }


    free(prompt_char);
    free(path_courant);
    free(path_precedent);
    
    return val_retour;
}