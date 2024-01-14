#include "../lib/fg.h"
#include "../lib/env.h"
#include "../lib/kill.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>


int fg(int job_id){
    if(!id_taken[job_id - 1]){
        perror("Le job n'existe pas.");
        return 1;
    }
    
    job *resume = NULL;
    for(int i = 0; i < cmp_jobs; ++i){
        if(jobs_suivis[i] -> id == job_id){
            resume = jobs_suivis[i];
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
    res = tcsetpgrp(default_fd[0], resume -> pgid);
    if(res != 0){
        perror("Erreur tcsetpgrp fg");
        return 1;
    }
    int info_fils;
    fprintf(stderr, "%s\n", resume -> cmd);

    info_fils = set_status(resume, false);
    if(info_fils == -1){
        perror("waitpid (fg)");
        return -1;
    }
    if(resume->jobstatus == JOB_DONE){
        suppresion_job(cmp_jobs - 1);
    }else{
        resume -> afficher_save = true;
    }

    res = tcsetpgrp(default_fd[0], getpid());
    if(res != 0){
        perror("Erreur tcsetpgrp fg");
        return 1;
    }
    return 0;
}