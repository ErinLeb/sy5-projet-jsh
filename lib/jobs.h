#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>
#include <stdbool.h>

enum JobStatus {
    JOB_RUNNING = 0,
    JOB_DONE = 1,
    JOB_STOPPED = 2,
    JOB_KILLED = 3,
    JOB_DETACHED = 4
};

struct job {
    int id;
    pid_t pgid;
    pid_t *pid_proc;
    int nb_proc;
    char * cmd;
    //permet de savoir si la mise à jour de l'état du processus a déjà été faite avant que l'affichage soit fait (cela arrive quand le job est initialisé)
    bool afficher_save;
    enum JobStatus jobstatus;
};

typedef struct job job;

/* Vérifie si les jobs en cours sont terminés et les affiche si c'est le cas*/
extern void check_jobs_info ();

/* Affiche les informations sur tous les jobs en cours */
extern int jobs ();

extern void add_proc_to_job(pid_t pid, pid_t pgid);

extern job *new_job(pid_t pid, pid_t pgid, char *cmd);

extern void suppresion_job(int i);

extern int set_status(job *, bool);

#endif