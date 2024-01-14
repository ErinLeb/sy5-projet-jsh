#include "../lib/kill.h"
#include "../lib/env.h"
#include "../lib/jobs.h"
#include <signal.h>
#include <errno.h>
#include <stdio.h>


int kill_proc(pid_t pid, int sig) {
    int res = kill(pid, sig);
    if(res < 0) {
        if(errno == EINVAL) {
            perror("Numéro de signal invalide.");
            return 1;
        }
        else if(errno == EPERM) {
            perror("Le processus appelant n'a pas l'autorisation d'envoyer un signal aux processus cibles.");
            return 1;
        }
        else if(errno == ESRCH) {
            perror("Le processus ou le groupe de processus n'existe pas.");
            return 1;
        } 
    }
    return 0;
}


int kill_term_proc(pid_t pid) {
    return kill_proc(pid, SIGTERM);
}


int kill_job(int job, int sig) {
    if(id_taken[job - 1]){
        pid_t job_pid = 0;
        for(int i = 0; i < cmp_jobs; ++i) {
            if(jobs_suivis[i]->id == job){
                job_pid = jobs_suivis[i]->pgid;
                break;
            }
        }

        int res = kill(-job_pid, sig);
        if(res < 0) {
            if(errno == EINVAL) {
                perror("Numéro de signal invalide.");
                return 1;
            }
            else if(errno == EPERM) {
                perror("Le processus appelant n'a pas l'autorisation d'envoyer un signal aux processus cibles.");
                return 1;
            }
            else if(errno == ESRCH) {
                perror("Le processus ou le groupe de processus n'existe pas.");
                return 1;
            }
        }
        return 0;
    }
    perror("Le job n'existe pas.");
    return 1;
}


int kill_term_job(int job) {
    return kill_job(job, SIGTERM);
}