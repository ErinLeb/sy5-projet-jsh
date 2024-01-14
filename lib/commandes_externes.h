#ifndef CMDEXT_H
#define CMDEXT_H

#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include "jobs.h"

extern char * concat (int argc, char *argv[]);
/* Appelle la commande externe passée en argument (en background si bg)*/
extern int cmd_ext(int argc, char* argv[], bool bg, pid_t pid, pid_t pgid, job * current_job);

#endif