#include "../lib/exit.h"
#include "../lib/env.h"
#include <stdio.h>


int exit_jsh(){
    return exit_jsh_with_arg(val_retour);
}


int exit_jsh_with_arg(int val){
    if(cmp_jobs != 0){
        fprintf(stderr, "There is %d job.\n", cmp_jobs);
        return 1;
    }
    boucle = false;
    return val;
}