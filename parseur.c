#include "lib/env.h"
#include "lib/cd.h"
#include "lib/pwd.h"
#include "lib/exit.h"
#include "lib/parseur.h"    
#include "lib/last_output.h"
#include "lib/commandes_externes.h"
#include "lib/jobs.h"
#include "lib/kill.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>


bool isInt (char * str) {
    if(str == NULL || strlen (str) == 0){
        return false;
    }
    int i = 0;
    while (str[i] != '\0'){
        if (isdigit (str[i]) || (i == 0 && str[i] == '-')){
            i++;
        }
        else {
            return false;
        }
    }
    return true;
}


void parseur(int argc, char **argv){ 
    bool cmd_find = false;

    if (strcmp(argv[0], "cd") == 0) { 
        cmd_find = true;
        appel_exit = false;
        if (argc == 1){
            val_retour = cd();
        }if (argc == 2){
            val_retour =  cd_with_arg(argv[1]);
        }if (argc > 2) {
            val_retour = 1;
            perror("Trop d'arguments ont été donnés.");
        }
    }

    if (strcmp(argv[0], "?") == 0) { 
        cmd_find = true;
        appel_exit = false;
        if (argc == 1){
            val_retour = last_output();
        }
        else {
            val_retour = 1;
            perror("Trop d'arguments ont été donnés.");
        }
    }

    if (strcmp(argv[0], "pwd") == 0) {  
        cmd_find = true;
        appel_exit = false;
        if (argc == 1){
            val_retour = pwd();
        }
        else {
            val_retour = 1;
            perror("Trop d'arguments ont été donnés.");
        }
    }

    if (strcmp(argv[0], "exit") == 0) {  
        cmd_find = true;
        if (argc == 1){
            val_retour = exit_jsh();
        }
        if (argc == 2 && isInt(argv[1])){
            val_retour = exit_jsh_with_arg(atoi(argv[1]));
        }
        if (argc > 2 || (argc == 2 && !isInt(argv[1]))) {
            val_retour = 1;
            perror("Argument(s) donné(s) invalide(s).");
        }
    }

    if (strcmp(argv[0], "jobs") == 0) { 
        cmd_find = true;
        appel_exit = false;
        if (argc == 1){
            val_retour = jobs();
        }
        else {
            val_retour = 1;
            perror("Trop d'arguments ont été donnés.");
        }
    }

    if (strcmp(argv[0], "kill") == 0) {
        cmd_find = true;
        appel_exit = false;
        
        if (argc < 2 || argc > 3) {
            val_retour = 1;
            perror("Nombre d'arguments invalide.");
        }
        else if (argc == 2 && argv[1][0] == '%') {
            char *num_job = &argv[1][1];
            if (!isInt(num_job)) {
                val_retour = 1;
                perror("Job invalide.");
            }
            else {
                val_retour = kill_term_job(atoi(num_job));
            }
        }
        else if (argc == 2 && isInt(argv[1])) {
            val_retour = kill_term_proc(atoi(argv[1]));
        }
        else if (argc == 3 && argv[1][0] == '-' && argv[2][0] == '%') {
            char *num_job = &argv[2][1];
            char *sig = &argv[1][1];
            if (!isInt(num_job) || !isInt(sig)) {
                val_retour = 1;
                perror("Job ou signal invalide.");
            }
            else {
                val_retour = kill_job(atoi(num_job), atoi(sig));
            }
        }
        else if (argc == 3 && argv[1][0] == '-' && isInt(argv[2])) {
            char *sig = &argv[1][1];
            if (!isInt(sig)) {
                val_retour = 1;
                perror("Signal invalide.");
            }
            else {
                val_retour = kill_proc(atoi(argv[2]), atoi(sig));
            }
        }
        else {
            perror("Argument(s) invalide(s).");
            val_retour = 1;
        }
    }

    if (!cmd_find){
        bool bg;
        appel_exit = false;
        if (strcmp(argv[argc-1], "&") == 0){
            argc--;
            argv[argc] = NULL;
            bg = true;
        }
        else {
            argv = realloc (argv, (argc + 1)*sizeof(char *));
            if (argv == NULL){
                val_retour = 1;
                perror("Erreur d'allocation parseur");
            }
            argv[argc] = NULL;
            bg = false;
        }
        val_retour = cmd_ext(argc, argv, bg);
    }
    free(argv);
}


void parseur_redirections(char *cmd){
    if (!cmd){
        val_retour = exit_jsh();
        return;
    }

    // variables parseur
    char *sep = " ";
    char *current = strtok(cmd, sep);
    char** argv = NULL;
    int argc = 0;

    // variables redirections
    int res; 
    int newfic, oldfic; 
    int flags; 
    bool redirection = false; 
    bool creat = true; // indique si le flag O_CREAT est présent
    bool changed[3] = {false, false, false}; // descripteurs changés

////////////////////////////////////////////////////////////////////////////////:
    //Variables pipe
    struct commande {
        char ** argv;
        int argc;
    };
    typedef struct commande commande;

    int nb_pipe = 0;
    commande *commandes = NULL; 
    commande *tmp = NULL;
    commande comm;
    comm.argc = 0;
    comm.argv = malloc(sizeof(char *));
////////////////////////////////////////////////////////////////////////////////:

    while(current != NULL){  
        // si strtok détecte un symbole >, >>, <, ...
        // il effectue le chgt de descripteur avec le prochain appel a strtok

        if(strcmp(current, "<") == 0){ // entrée standard
            redirection = true;
            changed[0] = true;
            creat = false;
            flags = O_RDONLY;
            oldfic = 0;
        }

        else if(strcmp(current, ">") == 0){ // sortie standard, échoue si le fichier existe déjà
            redirection = true;
            changed[1] = true;
            flags = O_WRONLY|O_CREAT|O_EXCL;
            oldfic = 1;
        }

        else if(strcmp(current, ">|") == 0){ // sortie standard, écrasement
            redirection = true;
            changed[1] = true;
            flags = O_WRONLY|O_CREAT|O_TRUNC;
            oldfic = 1;
        }

        else if(strcmp(current, ">>") == 0){ // sortie standard, concaténation
            redirection = true;
            changed[1] = true;
            flags = O_WRONLY|O_CREAT|O_APPEND;
            oldfic = 1;
        }

        else if(strcmp(current, "2>") == 0){ // sortie erreur 
            redirection = true;
            changed[2] = true;
            flags = O_WRONLY|O_CREAT|O_EXCL;
            oldfic = 2;
        }

        else if(strcmp(current, "2>|") == 0){ // sortie erreur, écrasement
            redirection = true;
            changed[2] = true;
            flags = O_WRONLY|O_CREAT|O_TRUNC;
            oldfic = 2;
        }

        else if(strcmp(current, "2>>") == 0){ // sortie erreur, concaténation
            redirection = true;
            changed[2] = true;
            flags = O_WRONLY|O_CREAT|O_APPEND;
            oldfic = 2;
        }

////////////////////////////////////////////////////////////////////////////////:
        // PIPE
        else if (strcmp(current, "|") == 0){
            nb_pipe++;
            tmp = realloc(commandes, (nb_pipe + 1) * sizeof(commande)); 
            if(tmp == NULL){
                val_retour = 1;
                perror("realloc pipe (parseur_redirections)");
                goto maj_default;
                return;
            }
            commandes = tmp;
            comm.argc = argc;
            comm.argv = memcpy(comm.argv, argv, sizeof(char *));
            if(comm.argv == NULL){
                val_retour = 1;
                perror("memcpy (pipe)");
                goto maj_default;
                return;
            }
            commandes[nb_pipe - 1] = comm; 
            argv = NULL;
            argc = 0;
        }
////////////////////////////////////////////////////////////////////////////////

        // sinon, il ajoute les éléments à la commande à passer au parseur
        else{
            redirection = false;
            argc ++;
            argv = realloc(argv, argc * sizeof(char *));
            if (argv == NULL){
                val_retour = 1;
                perror("realloc");
                goto maj_default;
                return;
            }
            argv[argc - 1] = current;
        }

////////////////////////////////////////////////////////////////////////////////
    /*if(nb_pipe > 0){ 
        commande cmd;
        cmd.argc = argc;
        strcpy(cmd.argv, argv);
        commandes[nb_pipe] = cmd;

        int fd[nb_pipe][2];

        for(int i = 0; i < nb_pipe; i++){
            pipe(fd[i]);
            //(commandes[i]) écrit dans fd[i][1], close fd[i][0]
            //commandes[i+1] lit dans fd[i][0], close fd[i][1]
            if(i != 0){ // fils déjà existant du tour précédent, redirection en lecture sur le pipe qui vient d'être ouvert
                
            }
            if(i == 0){
                if (fork() == 0){ // création d'un fils -> redirection écriture sur le pipe qui vient d'être ouvert
                    close(fd[i][0]);
                    dup2(fd[i][1], 1);
                    close(fd[i][0]);
                    argc = commandes[0].argc;
                    argv = commandes[0].argv;
                    break; //le premier fils sort de la boucle
                    //TODO : fork en trop ?????????
                }else{ //pere -> ajout du processus au job ?

                }
                if(fork() == 0){

                }
            }
            
            
        }*/
        if(nb_pipe == 1){
            int fd[2];
            pipe(fd);
            if(!fork()){// cmd1
                printf("fork fils\n");
                close(fd[0]);
                changed[1] = true;
                res = dup2(fd[1], 1);
                if(res < 0){
                    val_retour = 1;
                    perror("dup2 fils");
                    goto maj_default;
                    return;
                }
                close(fd[1]);
                argc = commandes[0].argc;
                argv = commandes[0].argv;
            }else{ //père : cmd2
                printf("fork pere\n");
                close(fd[1]);
                changed[0] = true;
                res = dup2(fd[0], 0);
                if(res < 0){
                    val_retour = 1;
                    perror("dup2 pere");
                    goto maj_default;
                    return;
                }
                close(fd[0]);
                argc = commandes[1].argc;
                argv = commandes[1].argv;
            }
        }
    

    //TODO : close les descripteurs restants !
////////////////////////////////////////////////////////////////////////////////:

        if(redirection){
            char * nom_fic = strtok(NULL, sep);
            if(creat){
                newfic = open(nom_fic, flags, 0664);
            }else{
                newfic = open(nom_fic, flags);
            }
            if(newfic == -1){
                val_retour = 1;
                perror("open");
                goto maj_default; 
                return;
            }     

            res = dup2(newfic, oldfic);
            if(res < 0){
                val_retour = 1;
                perror("dup2");
                goto maj_default;
                return;
            }
            close(newfic);
        }

        current = strtok(NULL, sep);
        creat = true;
    }

    if (argv == NULL){
        printf("argv == NULL\n");
        goto maj_default;
        return;
    }

    // On traite la commande
    parseur(argc, argv); //TODO : change
    printf("apres parseur\n");


    goto maj_default;

    // On remet les descripteurs par défaut
    maj_default:
    for(int i = 0; i < 3; i++){
        if(changed[i]){
            dup2(default_fd[i], i);
        }
    }
}