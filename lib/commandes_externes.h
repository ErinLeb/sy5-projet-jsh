#ifndef CMDEXT_H
#define CMDEXT_H

#include <stdbool.h>

/* Appelle la commande externe passée en argument */
extern int cmd_ext(int argc, char* argv[], bool bg);

#endif