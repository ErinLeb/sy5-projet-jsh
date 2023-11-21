#include "../lib/pwd.h"
#include "../lib/env.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int pwd() {
    int res = printf("%s\n", path_courant);
    if(res < 0){
        perror("Problème à l'affichage du path.\n");
        return 1;
    }
    return 0;
}