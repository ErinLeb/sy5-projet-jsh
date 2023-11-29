#ifndef ENV_H
#define ENV_H

#include <stdbool.h>

#define PATH_MAX 4096
#define PROMPT 52

extern char * path_courant;
extern char * path_precedent;
extern int jobs;
/* dernière valeur de retour de la commande exécutée par l'utilisateur */
extern int val_retour;  
/* booléen indiquant si on continue d'exécuter jsh*/
extern bool boucle; 

#endif