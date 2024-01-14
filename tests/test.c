#include "../lib/cd.h"
#include "../lib/env.h"
#include "../lib/pwd.h"
#include "../lib/exit.h"
#include "../lib/prompt.h"
#include "../lib/last_output.h"
#include "../lib/commandes_externes.h"
#include "../lib/jobs.h"
#include "../lib/kill.h"
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <signal.h>

//à changer si un test utilise des redirections
int default_fd [3] = {STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO}; 
char * path_courant;
char * path_precedent;
int cmp_jobs = 0; 
job * jobs_suivis[NBR_MAX_JOBS];
bool id_taken[NBR_MAX_JOBS];
int val_retour = 0;  
bool boucle = true;
bool appel_exit = false;

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
    cmp_jobs = 1;
    val_retour = exit_jsh();
    printf("Exit avec jobs en cours : %d -- toujours dans la boucle ? %d\n", val_retour, boucle);

    cmp_jobs = 0;
    val_retour = 0;
    val_retour = exit_jsh();
    printf("Exit (dernière valeur de retour 0) : %d -- toujours dans la boucle ? %d\n", val_retour, boucle);

    boucle = true;
    val_retour = exit_jsh_with_arg(42);
    printf("Exit avec valeur 42 : %d -- toujours dans la boucle ? %d\n", val_retour, boucle);
}

void test_prompt() {
    printf("-------------------------TEST PROMPT----------------------------\n");
    path_courant = malloc(sizeof(char) * (PATH_MAX + 1));
    rl_outstream = stderr;
    char * p = malloc(sizeof(char) * PROMPT);
    char * r;
    p[PROMPT - 1] = '\0';
    cmp_jobs = 0;
    memmove(path_courant,"/",2);
    p = prompt(p);
    r = readline(p);
    free(r);
    free(p);
    memmove(path_courant,"/court",7);
    p = malloc(sizeof(char) * PROMPT);
    p = prompt(p);
    r = readline(p);
    free(r);
    free(p);
    memmove(path_courant,"/trop/long/pour/etre/affiche/correctement",42);
    p = malloc(sizeof(char) * PROMPT);
    p = prompt(p);
    r = readline(p);
    free(r);
    free(p);
    free(path_courant);
}

void test_kill() {
    printf("-------------------------TEST KILL----------------------------\n");
    val_retour = kill_proc(getpid(), SIGTERM);
    printf("valeur attendue : 0, valeur obtenue: %d\n", val_retour);
    val_retour = kill_term_proc(getpid());
    printf("valeur attendue : 0, valeur obtenue: %d\n", val_retour);
    val_retour = kill_proc(getpid(), 80);
    printf("valeur attendue : 1, valeur obtenue: %d\n", val_retour);
}

int main(){
    struct sigaction ignore;
    memset(&ignore, 0, sizeof(struct sigaction));
    ignore.sa_handler = SIG_IGN;
    sigaction(SIGTERM, &ignore, NULL);

    test_pwd();
    printf("\n\n");
    
    test_last_output();
    printf("\n\n");

    test_exit();
    printf("\n\n");
    
    test_prompt();
    printf("\n\n");

    test_cmd_ext();
    printf("\n\n");

    test_kill();
    printf("\n\n");

    return 0;
}