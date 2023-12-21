#include "../lib/env.h"
#include "../lib/commandes_externes.h"
#include "../lib/jobs.h"
#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

char * concat (int argc, char *argv[]) {
    int taille = 0;
    for (int i = 0 ; i < argc; i++) {
        taille += strlen(argv[i]);
    }
    char* res = malloc ((taille + argc) * sizeof(char));
    res[0] = '\0';
    for (int i = 0 ; i < argc-1; i++) {
        strcat(res,argv[i]);
        strcat(res," ");
    }
    strcat(res,argv[argc - 1]);
    return res; 
}

int cmd_ext(int argc, char* argv[], bool bg){

    pid_t pid = fork();

    if (pid == -1){
        perror ("Erreur lors du fork cmd_ext.");
        return 1;
    }

    if (pid == 0){
        setpgid(0, 0);
        execvp(argv[0], argv);
        exit (1);
    }

    else {
        int status;
        int info_fils;
        char * cmd = concat(argc,argv);
        job * current_job = new_job(pid, cmd);
        pid_jobs[cmp_jobs] = current_job;
        free(cmd);
        cmp_jobs++;

        if (bg){
            current_job -> afficher_save = true;
            current_job -> jobstatus = JOB_RUNNING;
        }
        else {
            
            info_fils = waitpid(pid,&status,WUNTRACED);

            if (info_fils == -1)
            {
                perror("waitpid (cmd_ext)");
                return WEXITSTATUS(status);
            }
            
            if (WIFEXITED(status))
            {
                suppresion_job(cmp_jobs-1);
            } else {
                current_job -> afficher_save = true;
                set_status(current_job,status);
            }
        }
        return WEXITSTATUS(status);
    }
}