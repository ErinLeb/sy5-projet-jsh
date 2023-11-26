#include "../lib/exit.h"
#include "../lib/env.h"
#include <stdio.h>

int exit_jsh(){
    return exit_jsh_with_arg(val_retour);
}

int exit_jsh_with_arg(int val){
    if(jobs != 0){
        printf("Des jobs sont en cours d'exécution ou suspendus.\n");
        return 1;
    }
    boucle = false;
    return val;
}
