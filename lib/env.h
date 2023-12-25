#ifndef ENV_H
#define ENV_H

#include "jobs.h"
#include <stdbool.h>

#define PATH_MAX 4096
#define PROMPT 52
#define NBR_MAX_JOBS 1024

// Entrée et sorties par défaut 
extern int default_fd [3];

extern char * path_courant;
extern char * path_precedent;
extern int cmp_jobs;
extern job * pid_jobs[];
extern bool id_taken[];
/* dernière valeur de retour de la commande exécutée par l'utilisateur */
extern int val_retour;  
/* booléen indiquant si on continue d'exécuter jsh*/
extern bool boucle;
extern bool appel_exit;

#endif