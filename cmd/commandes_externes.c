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


int cmd_ext(int argc, char* argv[], bool bg){
    pid_t pid = fork();

    if (pid == -1){
        perror ("Erreur lors du fork cmd_ext.");
        return 1;
    }

    if (pid == 0){
        struct sigaction def = {0};
        def.sa_handler = SIG_DFL;
        for(int i = 1; i < NSIG; ++i){
            sigaction(i, &def, NULL);
        }
        execvp(argv[0], argv);
        exit (1);
    }

    else {
        int res = setpgid(pid, pid);
        if (res == -1){
            perror("setpgid");
            return 1;
        }
        int status;
        int info_fils;
        char * cmd = concat(argc, argv);
        job * current_job = new_job(pid, cmd);
        pid_jobs[cmp_jobs] = current_job;
        free(cmd);
        cmp_jobs++;

        if (bg){
            current_job -> afficher_save = true;
            current_job -> jobstatus = JOB_RUNNING;
        
        } 
        else {
            res = tcsetpgrp(default_fd[0],pid);
            if (res == -1){
                perror("tcsetpgrp cmd_ext");
                return 1;
            }

            info_fils = waitpid(pid, &status, WUNTRACED);

            if (info_fils == -1){
                perror("waitpid (cmd_ext)");
                return WEXITSTATUS(status);
            }
            
            if (WIFEXITED(status)){
                suppresion_job(cmp_jobs - 1);
            } 
            else {
                current_job -> afficher_save = true;
                set_status(current_job, status);
            }

            res = tcsetpgrp(default_fd[0],getpid());
            if (res == -1){
                perror("tcsetpgrp jsh ");
                return 1;
            }
        }
        return WEXITSTATUS(status);
    }
}