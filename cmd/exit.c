#include "../lib/exit.h"
#include "../lib/env.h"
#include <stdio.h>

void exit_jsh(){
    if(jobs != 0){
        printf("Des jobs sont en cours d'exécution ou suspendus.\n");
        val_retour = 1;
        return;
    }
    boucle = false;
}
void exit_jsh_with_arg(int val){
    val_retour = val;
    exit_jsh();
}