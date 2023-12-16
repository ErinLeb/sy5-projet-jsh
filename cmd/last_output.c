#include "../lib/last_output.h"
#include "../lib/env.h"
#include <stdio.h>


int last_output() {
    int res = printf("%d\n", val_retour);

    if(res < 0) {
        perror("Erreur d'affichage dans last_output.");
        return 1;
    }

    return 0;
}