#include "lib/env.h"

int main(int argc, char const *argv[])
{
    courant = opendir("/");
    path_courant = "/";
    jobs=0;
    val_retour = 0;
    boucle = true;
    closedir (courant);
    return val_retour;
}
