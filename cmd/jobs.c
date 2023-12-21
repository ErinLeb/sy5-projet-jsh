#include "../lib/env.h"
#include "../lib/jobs.h"
#include "../lib/parseur.h"
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

job *new_job(pid_t pid, char *cmd){
    job *res = malloc(sizeof(int) + sizeof(pid_t) + (strlen(cmd) + 1) * sizeof(char) + sizeof(bool) + sizeof(enum JobStatus));
    if (res == NULL){
        perror("malloc (new_job)");
    }
    int id;
    for (id = 0; id < NBR_MAX_JOBS; id++){
        if (!id_taken[id]){
            id_taken[id] = true;
            break;
        }
    }
    res->id = id + 1;
    res->pid = pid;
    res->cmd = malloc((strlen(cmd) + 1) * sizeof(char));
    strcpy(res->cmd, cmd);
    res->afficher_save = false;
    return res;
}

void suppresion_job(int i){
    job *current_job = pid_jobs[i];
    id_taken[(current_job->id) - 1] = false;
    free(current_job->cmd);
    free(current_job);
    for (int j = i; j < cmp_jobs - 1; j++){
        pid_jobs[j] = pid_jobs[j + 1];
    }
    cmp_jobs--;
}

void set_status(job * j, int status){
    if (WIFCONTINUED(status)){
        j->jobstatus = JOB_RUNNING;
    }
    else if (WIFEXITED(status)){
        j->jobstatus = JOB_DONE;
    }
    else if (WIFSTOPPED(status)){
        j->jobstatus = JOB_STOPPED;
    }
    else if (WIFSIGNALED(status)){
        j->jobstatus = JOB_KILLED;
    }
    else{
       j->jobstatus =  JOB_DETACHED;
    }
}

void check_jobs_info(){
    job *current_job;
    int status;
    int info_waitpid;

    for (int i = 0; i < cmp_jobs; i++){
        current_job = pid_jobs[i];

        if (current_job -> afficher_save) {
            current_job -> afficher_save = false;
            info_waitpid = 1;
        }
        else {
            info_waitpid = waitpid(current_job->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
            if (info_waitpid == -1){
                perror("waitpid (check_jobs_info)");
                return;
            }
            if (info_waitpid != 0){
                set_status (current_job, status);
            }
        }

        if (info_waitpid != 0){
            if (current_job -> jobstatus == JOB_RUNNING){
                fprintf(stderr, "[%i] %i Running        %s\n", current_job->id, current_job->pid, current_job->cmd);
            }
            else if (current_job -> jobstatus == JOB_STOPPED){
                fprintf(stderr, "[%i] %i Stopped        %s\n", current_job->id, current_job->pid, current_job->cmd);
            }
            else if (current_job -> jobstatus == JOB_DONE){
                fprintf(stderr, "[%i] %i Done        %s\n", current_job->id, current_job->pid, current_job->cmd);
                suppresion_job(i);
                i--;
            }
            else if (current_job -> jobstatus == JOB_KILLED){
                fprintf(stderr, "[%i] %i Killed        %s\n", current_job->id, current_job->pid, current_job->cmd);
                suppresion_job(i);
                i--;
            }
            else{
                fprintf(stderr, "[%i] %i Detached        %s\n", current_job->id, current_job->pid, current_job->cmd);
                suppresion_job(i);
                i--;
            }
        }
    }
}

int jobs(){
    job *current_job;
    int info_waitpid;
    int status;
    for (int i = 0; i < cmp_jobs; i++){
        current_job = pid_jobs[i];

        info_waitpid = waitpid(current_job->pid, &status, WNOHANG | WUNTRACED | WCONTINUED);

        if (info_waitpid != 0){
            set_status (current_job, status);
        }
        
        if (current_job -> jobstatus == JOB_RUNNING){
            printf("[%i] %i Running        %s\n", current_job->id, current_job->pid, current_job->cmd);
        }
        else if (current_job -> jobstatus == JOB_DONE){
            printf("[%i] %i Done        %s\n", current_job->id, current_job->pid, current_job->cmd);
            suppresion_job(i);
            i--;
        }
        else if (current_job -> jobstatus == JOB_STOPPED){
            printf("[%i] %i Stopped        %s\n", current_job->id, current_job->pid, current_job->cmd);
        }
        else if (current_job -> jobstatus == JOB_KILLED){
            printf("[%i] %i Killed        %s\n", current_job->id, current_job->pid, current_job->cmd);
            suppresion_job(i);
            i--;
        } 
        else{
            printf("[%i] %i Detached        %s\n", current_job->id, current_job->pid, current_job->cmd);
            suppresion_job(i);
            i--;
        }
    }
    return 0;
}