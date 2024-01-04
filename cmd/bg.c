#include "../lib/bg.h"
#include "../lib/env.h"
#include "../lib/kill.h"
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>


int bg(int job_id){
    if(!id_taken[job_id - 1]){
        perror("Le job n'existe pas.");
        return 1;
    }
    
    job *resume = NULL;
    for(int i = 0; i < cmp_jobs; ++i){
        if(pid_jobs[i] -> id == job_id){
            resume = pid_jobs[i];
            break;
        }
    }
    if(resume -> jobstatus != JOB_STOPPED){
        perror("Le processus est déjà en cours.");
        return 1;
    }
    
    int res = kill_job(job_id, SIGCONT);
    if(res != 0){
        perror("erreur kill_job");
        return 1;
    }
    resume -> jobstatus = JOB_RUNNING;
    return 0;
}