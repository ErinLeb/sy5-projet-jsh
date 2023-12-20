#ifndef KILL_H
#define KILL_H

#include <sys/types.h>

/* Envoie le signal @param sig au processus de pid @param pid */
extern int kill_proc(pid_t pid, int sig);

/* Envoie le signal SIGTERM au processus de pid @param pid */
extern int kill_term_proc(pid_t pid);

/* Envoie le signal @param sig à tous les processus du job numéro @param job */
extern int kill_job(int job, int sig);

/* Envoie le signal SIGTERM à tous les processuss du job numéro @param job*/
extern int kill_term_job(int job);

#endif