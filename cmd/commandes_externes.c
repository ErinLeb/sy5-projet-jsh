#include "../lib/env.h"
#include "../lib/commandes_externes.h"
#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int cmd_ext(char * argv[]){
    pid_t pid = fork();
    if (pid == -1){
        perror ("Erreur lors du fork cmd_ext.");
        return 1;
    }
    if (pid == 0){
        return execvp(argv[0], argv);
    }
    else {
        int status;
        waitpid(pid,&status,0);
        if (!WIFEXITED(status)){
            perror ("Erreur lors du execv cmd_ext.");
        }
        return WEXITSTATUS(status);
    }
}