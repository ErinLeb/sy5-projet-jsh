#include "../lib/env.h"
#include "../lib/jobs.h"
#include "../lib/parseur.h"
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

job *new_job(pid_t pid, char *cmd)
{
    job *res = malloc(sizeof(int) + sizeof(pid_t) + (strlen(cmd) + 1) * sizeof(char));
    int id;
    for (id = 0; id < NBR_MAX_JOBS; id++)
    {
        if (!id_taken[id])
        {
            id_taken[id] = true;
            break;
        }
    }
    res->id = id + 1;
    res->pid = pid;
    res->cmd = malloc((strlen(cmd) + 1) * sizeof(char));
    strcpy(res->cmd, cmd);
    setpgid(pid, 0);
    return res;
}

/*Méthode qui prend une liste de char* et renvoie leur concaténation*/
char *concat(int argc, char *argv[])
{
    int taille = 0;
    for (int i = 0; i < argc; i++)
    {
        taille += strlen(argv[i]);
    }
    char *res = malloc(taille + 1);
    strcpy(res, "");
    for (int i = 0; i < argc - 1; i++)
    {
        strcat(res, argv[i]);
        strcat(res, " ");
    }
    strcat(res, argv[argc - 1]);
    return res;
}

int init_job(int argc, char *argv[])
{
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("Erreur lors du fork d'initialisation du jobs.");
        return 1;
    }

    if (pid == 0)
    {
        execvp(argv[0], argv);
        exit(1);
    }

    else
    {
        pid_jobs[cmp_jobs] = new_job(pid, concat(argc, argv));
        fprintf(stderr, "[%i] %i Running        %s\n", pid_jobs[cmp_jobs]->id, pid, pid_jobs[cmp_jobs]->cmd);
        cmp_jobs++;
        return 0;
    }
}

void suppresion_job(int i)
{
    job *current_job = pid_jobs[i];
    id_taken[(current_job->id) - 1] = false;
    free(current_job->cmd);
    free(current_job);
    for (int j = i; j < cmp_jobs - 1; j++)
    {
        pid_jobs[j] = pid_jobs[j + 1];
    }
    cmp_jobs--;
}

void check_jobs_info()
{
    job *current_job;
    int status;
    int info_fils;

    for (int i = 0; i < cmp_jobs; i++)
    {
        current_job = pid_jobs[i];
        info_fils = waitpid(current_job->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);

        if (info_fils == -1)
        {
            perror("waitpid (check_jobs_info)");
            return;
        }

        if (info_fils != 0)
        {
            if (WIFEXITED(status))
            {
                fprintf(stderr, "[%i] %i Done        %s\n", current_job->id, current_job->pid, current_job->cmd);
                suppresion_job(i);
                i--;
            }
            else if (WIFSIGNALED(status))
            {
                fprintf(stderr, "[%i] %i Killed        %s\n", current_job->id, current_job->pid, current_job->cmd);
                suppresion_job(i);
                i--;
            }
            else if (WIFSTOPPED(status))
            {
                fprintf(stderr, "[%i] %i Stopped        %s\n", current_job->id, current_job->pid, current_job->cmd);
            }
            else
            {
                fprintf(stderr, "[%i] %i Detached        %s\n", current_job->id, current_job->pid, current_job->cmd);
                suppresion_job(i);
                i--;
            }
        }
    }
}

int jobs()
{
    int info_fils;
    int status;
    job *current_job;
    for (int i = 0; i < cmp_jobs; i++)
    {
        current_job = pid_jobs[i];
        info_fils = waitpid(current_job->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);

        if (info_fils == -1)
        {
            perror("waitpid (demande_jobs_info)");
            return 1;
        }

        if (info_fils == 0)
        {
            printf("[%i] %i Running        %s\n", current_job->id, current_job->pid, current_job->cmd);
        }
        else
        {
            if (WIFEXITED(status))
            {
                printf("[%i] %i Done        %s\n", current_job->id, current_job->pid, current_job->cmd);
                suppresion_job(i);
                i--;
            }
            else if (WIFSIGNALED(status))
            {
                printf("[%i] %i Killed        %s\n", current_job->id, current_job->pid, current_job->cmd);
                suppresion_job(i);
                i--;
            }
            else if (WIFSTOPPED(status))
            {
                printf("[%i] %i Stopped        %s\n", current_job->id, current_job->pid, current_job->cmd);
            }
            else
            {
                printf("[%i] %i Detached        %s\n", current_job->id, current_job->pid, current_job->cmd);
            }
        }
    }
    return 0;
}