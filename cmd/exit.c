#include "../lib/exit.h"
#include "../lib/env.h"
#include "../lib/kill.h"
#include <stdio.h>
#include <signal.h>


int exit_jsh(){
    return exit_jsh_with_arg(val_retour);
}


int exit_jsh_with_arg(int val){
    if(cmp_jobs != 0 && !appel_exit){
        appel_exit = true;
        fprintf(stderr, "There is %d job.\n", cmp_jobs);
        return 1;
    }
    for(int i = 0; i < cmp_jobs; ++i){
        if(jobs_suivis[i]->jobstatus == 2){
            kill_job(jobs_suivis[i]->id, SIGCONT);
        }
        kill_job(jobs_suivis[i]->id, SIGHUP);
    }
    boucle = false;
    return val;
}