#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>
#include <stdbool.h>

struct job
{
    int id;
    pid_t pid;
    char * cmd;
};

typedef struct job job;

/*Initialise un jobs à partir d'une commande donnée*/
extern int init_job(int argc, char *argv[]);

/*Vérifie si les jobs en cours sont terminés et les affiches si c'est le cas*/
extern void check_jobs_info ();

#endif