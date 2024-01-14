#include "../lib/commandes_externes.h"
#include "../lib/env.h"
#include "../lib/jobs.h"
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>


char * concat (int argc, char *argv[]) {
    int taille = 0;
    for (int i = 0; i < argc; i++) {
        taille += strlen(argv[i]);
    }
    char* res = malloc ((taille + argc) * sizeof(char));
    res[0] = '\0';
    for (int i = 0; i < argc-1; i++) {
        strcat(res, argv[i]);
        strcat(res, " ");
    }
    strcat(res, argv[argc - 1]);
    return res; 
}


int cmd_ext(int argc, char* argv[], bool bg, pid_t pid){

    if (pid == 0){
        struct sigaction def = {0};
        def.sa_handler = SIG_DFL;
        for(int i = 1; i < NSIG; ++i){
            sigaction(i, &def, NULL);
        }
        execvp(argv[0], argv);
        perror("exec failed");
        exit (1);
    }

    int res = setpgid(pid, pid); //TODO à généraliser pour les pipes
    if (res == -1){
        perror("setpgid");
        return 1;
    }
    
    char * cmd = concat(argc, argv);
    job * current_job = new_job(pid, cmd);
    jobs_suivis[cmp_jobs] = current_job;
    cmp_jobs++;
    //add_proc_to_job(pid,pid); //TODO : change pour les pipes
    free(cmd);
    int exitedstatus = current_job -> exitedstatus;

    if (bg){
        current_job -> afficher_save = true;
        current_job -> jobstatus = JOB_RUNNING;
    }
    else {
        res = tcsetpgrp(default_fd[0], pid);
        if (res == -1){
            perror("tcsetpgrp cmd_ext");
            return 1;
        }   
        
        current_job -> jobstatus = JOB_RUNNING;
        
        if(set_status(current_job, bg) < 0){
            perror("wait cmd_ext");
            return 1;
        }

        exitedstatus = current_job -> exitedstatus;
        
        if(current_job->jobstatus != JOB_RUNNING && current_job->jobstatus != JOB_STOPPED){
            suppresion_job(cmp_jobs - 1);
        }else{
            current_job->afficher_save = true;
        }

        res = tcsetpgrp(default_fd[0], getpgid(getpid()));
        if (res == -1){
            perror("tcsetpgrp jsh");
            return 1;
        }
    }
    return exitedstatus;
}