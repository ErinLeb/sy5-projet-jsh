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


int cmd_ext(int argc, char* argv[], bool bg, pid_t pid, pid_t pgid, job * current_job){

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

    int res = setpgid(pid, pgid); 
    if (res == -1){
        perror("setpgid");
        return 1;
    }
    return 0;
}