#ifndef ENV_H
#define ENV_H

#include <stdbool.h>

#define PATH_MAX 4096

extern char * path_courant;
extern char * path_precedent;
extern int jobs;
extern int val_retour;  /* dernière valeur de retour de la commande exécutée par l'utilisateur */
extern bool boucle; /* booléen indiquant si on continue d'exécuter jsh*/

#endif