#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "lib/parseur.h"    
#include "lib/env.h"
#include "lib/cd.h"
#include "lib/last_output.h"
#include "lib/pwd.h"
#include "lib/exit.h"
#include "lib/commandes_externes.h"

bool isInt (char * str) {
    if(str == NULL || strlen (str) == 0){
        return false;
    }
    while (*str != '\0'){
        if (!(isdigit (*str))){
            return false;
        }
        str++;
    }
    return true;
}

void parseur(char cmd[]){ 
    if (!cmd){
        val_retour = exit_jsh();
        return;
    }
    char *sep = " ";
    char *current = strtok(cmd, sep);
    char** argv = NULL;
    int argc = 0;
    bool cmd_find = false;
    while(current != NULL)
    {   
        argc ++;
        argv = realloc (argv, argc*sizeof(char*));
        if (argv == NULL){
            val_retour = 1;
            perror("Erreur d'allocation parseur");
            return;
        }
        argv[argc - 1] = current;
        current = strtok(NULL, sep);
    }
    if (argv == NULL){
        val_retour = 1;
        perror("Problème d'allocation parseur, aucun argument n'a été trouvé.");
        return;
    }
    if (strcmp(argv[0],"cd") == 0) { 
        cmd_find = true;
        if (argc == 1){
            val_retour = cd();
        }if (argc == 2){
            val_retour =  cd_with_arg(argv[1]);
        }if (argc > 2) {
            val_retour = 1;
            perror("Trop d'arguments ont été donnés.");
        }
    }
    if (strcmp(argv[0],"?") == 0) { 
        cmd_find = true;
        if (argc == 1){
            val_retour = last_output();
        }
        else {
            val_retour = 1;
            perror("Trop d'arguments ont été donnés.");
        }
    }
    if (strcmp(argv[0],"pwd") == 0) {  
        cmd_find = true;
        if (argc == 1){
            val_retour = pwd();
        }
        else {
            val_retour = 1;
            perror("Trop d'arguments ont été donnés.");
        }
    }
    if (strcmp(argv[0],"exit") == 0) {  
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
    if (!cmd_find){
        argv = realloc (argv, (argc+1)*sizeof(char*));
        if (argv == NULL){
            val_retour = 1;
            perror("Erreur d'allocation parseur");
        }
        argv[argc] = NULL;
        val_retour = cmd_ext(argv);
    }
    free(argv);
}
