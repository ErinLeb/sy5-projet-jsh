#ifndef ENV_H
#define ENV_H

#include <dirent.h>
#include <stdbool.h>

extern DIR * courant;
extern char * path_courant;
extern int jobs;
extern int val_retour;  /* dernière valeur de retour de la commande exécutée par l'utilisateur */
extern bool boucle; /* booléen indiquant si on continue d'exécuter jsh*/

#endif