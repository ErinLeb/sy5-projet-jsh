#include "lib/env.h"
#include "lib/cd.h"
#include "lib/pwd.h"
#include "lib/exit.h"
#include "lib/parseur.h"    
#include "lib/last_output.h"
#include "lib/commandes_externes.h"
#include "lib/jobs.h"
#include "lib/kill.h"
#include "lib/fg.h"
#include "lib/bg.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>


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



void parseur(int argc, char **argv, bool is_bg){ 
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
        else if (argc == 2 && strcmp(argv[1], "-t") == 0){
            val_retour = jobs_t();
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

    if(strcmp(argv[0], "fg") == 0){
        appel_exit = false;
        cmd_find = true;
        
        if(argc != 2){
            perror("Nombre d'arguments invalide");
            val_retour = 1;
        }
        char *num_job = &argv[1][1];
        if(argv[1][0] != '%' || !isInt(num_job)){
            perror("Argument invalide");
            val_retour = 1;
        }
        else{
            val_retour = fg(atoi(num_job));
        }
    }

    if(strcmp(argv[0], "bg") == 0){
        appel_exit = false;
        cmd_find = true;
        if(argc != 2){
            perror("Nombre d'arguments invalide.");
            val_retour = 1;
        }
        char *num_job = &argv[1][1];
        if(argv[1][0] != '%' || !isInt(num_job)){
            perror("Arguments invalides.");
            val_retour = 1;
        }
        else{
            val_retour = bg(atoi(num_job));
        }
    }

    if (!cmd_find){
        appel_exit = false;
        argv = realloc (argv, (argc+1)*sizeof(char*));
        if (argv == NULL){
            val_retour = 1;
            perror("Erreur d'allocation parseur");
        }
        argv[argc] = NULL;

        pid_t pid = fork();

        if (pid == -1){
            perror ("Erreur lors du fork cmd_ext.");
            val_retour = 1;
        }
        else {
            val_retour = cmd_ext(argc, argv, is_bg, pid);
        }
    }
    free(argv);
}


void is_bg(char *cmd){
    if (!cmd){
        val_retour = exit_jsh();
        return;
    }
    if(strcmp(cmd, "") == 0){
        return;
    }

    bool bg;
    int i = -1;
    char c; 
    int length = strlen(cmd);

    do{
        i++;
        c = cmd[length - 1 - i]; 
    }while(i < length - 1 && c == ' ');

    if(c == '&'){
        bg = true;
        char new_cmd[length];
        strcpy(new_cmd, cmd);
        new_cmd[length - 1 - i] = ' ';
        parseur_redirections(new_cmd, bg);
    
    }else{
        bg = false;
        parseur_redirections(cmd, bg);
    }
}

    
void parseur_redirections(char *cmd, bool bg){
    // variables parseur
    char *sep = " ";
    char *current = strtok(cmd, sep);
    char** argv = malloc(sizeof(char *));
    int argc = 0;

    // variables redirections
    int res; 
    int newfic, oldfic; 
    int flags; 
    bool redirection = false; 
    bool creat = true; // indique si le flag O_CREAT est présent
    bool changed[3] = {false, false, false}; // descripteurs changés

    //Variables substitution
    bool substitution = false;
    char *name_fifo;
    int tmp_fifo = 0;
    int fifo = 0;
    int fd_fifo_ecriture;

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

        else if(strcmp(current, "|") == 0){ // pipe
            redirection = false;
            int fd[2];
            res = pipe(fd);
            if(res < 0){
                perror("pipe");
                val_retour = 1;
                goto maj_default;
                return;
            }

            pid_t pid  = fork();
            if(pid < 0 ){
                perror("fork pipe");
                val_retour = 1;
                goto maj_default;
                return;
            }

            if(pid == 0){
                res = close(fd[0]);
                if(res < 0){
                    val_retour = 1;
                    perror("close");
                    goto maj_default;
                    return;
                }
                res = dup2(fd[1], 1);
                if(res < 0){
                    val_retour = 1;
                    perror("dup2");
                    goto maj_default;
                    return;
                }
                res = close(fd[1]);
                if(res < 0){
                    val_retour = 1;
                    perror("close");
                    goto maj_default;
                    return;
                }
                changed[1] = true;
                argv = realloc (argv, (argc+1)*sizeof(char*));
                if (argv == NULL){
                    val_retour = 1;
                    perror("Erreur d'allocation parseur");
                }
                argv[argc] = NULL;
                val_retour = cmd_ext(argc, argv, bg, pid);
            }else{
                res = close(fd[1]);
                if(res < 0){
                    val_retour = 1;
                    perror("close");
                    goto maj_default;
                    return;
                }
                res = dup2(fd[0], 0);
                if(res < 0){
                    val_retour = 1;
                    perror("dup2");
                    goto maj_default;
                    return;
                }
                res = close(fd[0]);
                if(res < 0){
                    val_retour = 1;
                    perror("close");
                    goto maj_default;
                    return;
                }
                changed[0] = true;

                //on remet argc et argv à 0
                argc = 0;
                free(argv);
                argv = malloc(sizeof(char *));
            }
        }

        else if(strcmp(current, "<(") == 0){
            substitution = true;
            redirection = false;
            // On alloue le bon nombre de caractères au nom du tube.
            int dizaine = 1;
            tmp_fifo = fifo;
            while(tmp_fifo / 10 > 0){
                ++dizaine;
                tmp_fifo /= 10;
            }
            name_fifo = malloc(sizeof(char) * (dizaine + 1));
            sprintf(name_fifo, "%d", fifo);
            mkfifo(name_fifo,0777);
            fifo++;

            // Réécriture de strcat
            char *cmd2 = malloc(sizeof(char));
            int index = 0;
            int len_cmd2 = 0;
            int len_current;
            if(cmd2 == NULL){
                perror("malloc cmd2");
                val_retour = 1;
                goto maj_default;
                return;
            }
            current = strtok(NULL, sep);
            while(strcmp(current, ")") != 0){
                len_current = strlen(current);
                len_cmd2 += len_current + 1;
                cmd2 = realloc(cmd2, sizeof(char) * (len_cmd2 + 1));
                if(cmd2 == NULL){
                    perror("realloc cmd2");
                    val_retour = 1;
                    goto maj_default;
                    return;
                }
                for(int i = 0; i < len_current; ++i){
                    cmd2[i + index] = current[i];
                }
                cmd2[len_current + index] = ' ';
                index = len_cmd2;
                current = strtok(NULL, sep);
            }
            cmd2[len_cmd2 - 1] = '\0';

            // Exécution des commandes
            res = fork();

            if(res == 0){ // cmd2
                fd_fifo_ecriture = open(name_fifo, O_WRONLY);
                if(fd_fifo_ecriture < 0){
                    perror("ouverture WR (substitution)");
                    exit(1);
                } 
                res = dup2(fd_fifo_ecriture, 1);
                if(res < 0){
                    perror("dup2 substitution");
                    exit(1);
                }
                res = close(fd_fifo_ecriture);
                if(res < 0){
                    perror("fermeture WR (substitution)");
                    exit(1);
                }
                parseur_redirections(cmd2, bg); // on suppose que la sortie standard est toujours le fifo
                free(cmd2);
                exit(val_retour);
            }else{ // cmd1
                free(cmd2);
                // name_fifo contient le résultat de cmd2
                argc ++;
                argv = realloc(argv, argc * sizeof(char *));
                if (argv == NULL){
                    val_retour = 1;
                    perror("realloc (parseur_redirections)");
                    goto maj_default;
                    return;
                }
                argv[argc - 1] = name_fifo;
            }
        }

        // sinon, il ajoute les éléments à la commande à passer au parseur
        else{
            redirection = false;
            argc ++;
            argv = realloc(argv, argc * sizeof(char *));
            if (argv == NULL){
                val_retour = 1;
                perror("realloc");
                return;
            }
            argv[argc - 1] = current;
        }

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
        goto maj_default;
        return;
    }

    // On traite la commande
    parseur(argc, argv, bg);

    goto maj_default;

    // On remet les descripteurs par défaut
    maj_default:
    for(int i = 0; i < 3; i++){
        if(changed[i]){
            res = close(i);
            if(res < 0){
                perror("close");
                exit(1);
            }
            res = dup2(default_fd[i], i);
            if(res < 0){
                perror("close");
                exit(1);
            }
        }
    }
    if(substitution){
        unlink(name_fifo);
        free(name_fifo);
    }
}