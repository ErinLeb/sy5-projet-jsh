#include "../lib/env.h"
#include "../lib/jobs.h"
#include "../lib/parseur.h"
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>


void add_proc_to_job(pid_t pid, pid_t pgid){
    for (int i = 0; i < NBR_MAX_JOBS; i++){ // pourquoi cmp_jobs ?
        job *j = jobs_suivis[i];
        if(j->pgid == pgid){
            j->nb_proc ++;
            j->pid_proc = realloc(j->pid_proc, j->nb_proc*sizeof(pid_t)); 
            j->pid_proc[j->nb_proc-1] = pid;
            j->status_proc = realloc(j->status_proc, j->nb_proc*sizeof(enum JobStatus));
            j->status_proc[j->nb_proc-1] = JOB_RUNNING;
            return;
        }
    }
    //TODO : erreur
    perror("groupe de job inexistant");
}


job *new_job(pid_t pgid, char *cmd){
    job *res = malloc(sizeof(job));
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
    res->exitedstatus = 0;
    res->pgid = pgid;
    res->nb_proc = 1;
    res->pid_proc = malloc(sizeof(pid_t));
    res->pid_proc[0] = pgid;
    res->status_proc = malloc(sizeof(enum JobStatus *));
    res->status_proc[0] = JOB_RUNNING;
    res->cmd_proc = malloc(sizeof(char **)); //TODO : à remplir
    res->cmd = malloc((strlen(cmd) + 1) * sizeof(char));
    strcpy(res->cmd, cmd);
    res->afficher_save = false;
    return res;
}


void suppresion_job(int i){
    job *current_job = jobs_suivis[i];
    id_taken[(current_job->id) - 1] = false;
    free(current_job->cmd);
    free(current_job->pid_proc);
    free(current_job->status_proc);
    free(current_job->cmd_proc);
    free(current_job);
    for (int j = i; j < cmp_jobs - 1; j++){
        jobs_suivis[j] = jobs_suivis[j + 1];
    }
    cmp_jobs--;
}

/**si le statut n'a pas changé, renvoie 0, sinon renvoie 1*/
int set_status(job * j,bool bg){
    int info_wait;
    int status;
    int pid;
    bool status_changed = true;
    
    int running = 0;
    int done = 0;
    int stopped = 0;
    int killed = 0;

    for(int i = 0; i < j->nb_proc; i++){
        if (j->status_proc[i] == JOB_DONE || j->status_proc[i] == JOB_KILLED){
            continue;
        }
        
        pid = j->pid_proc[i];
        if (bg){
            info_wait = waitpid(pid, &status, WCONTINUED | WNOHANG | WUNTRACED);
        }
        else{
            info_wait = waitpid(pid, &status, WUNTRACED);
        }
        if (info_wait == -1){
            perror("waitpid (set_status)");
            return -1;
        }

        if(info_wait == 0){
            status_changed = false;
        }
        else {
            if (WIFSIGNALED(status)){ //killed
                killed ++;
                done ++;  
                j->status_proc[i] = JOB_KILLED;
                j->exitedstatus = WEXITSTATUS(status);
            }
            else if (WIFEXITED(status)){ //done
                done ++;
                j->status_proc[i] = JOB_DONE; 
                j->exitedstatus = WEXITSTATUS(status);
            }
            
            else if (WIFSTOPPED(status)){ //stopped
                stopped ++;
                running++;
                j->status_proc[i] = JOB_STOPPED;
            }else {
                running ++;
                j->status_proc[i] = JOB_RUNNING;
            }
        }
    }

    if(!status_changed){
        return 0;
    }

    if(done == j->nb_proc && killed > 0){
        if (j->jobstatus != JOB_KILLED){
            status_changed = true;
            j->jobstatus = JOB_KILLED;
        }
    }else if(done == j->nb_proc){
        if (j->jobstatus != JOB_DONE){
            status_changed = true;
            j->jobstatus = JOB_DONE;
        }
    } 
    // else if(done == j->nb_proc){
    //     j->jobstatus = JOB_DETACHED;
    // }
    else if(stopped == running && stopped > 0){
        if (j->jobstatus != JOB_STOPPED){
            status_changed = true;
            j->jobstatus = JOB_STOPPED;
        }
    }else {
        if (j->jobstatus != JOB_RUNNING){
            status_changed = true;
            j->jobstatus = JOB_RUNNING;
        }
    }

    if(status_changed){
        return 1;
    }
    return 0;
}


void check_jobs_info(){
    job *current_job;

    for (int i = 0; i < cmp_jobs; i++){
        current_job = jobs_suivis[i];

        int res = 1;

        if (current_job -> afficher_save) {
            current_job -> afficher_save = false;
        }
        else {
            res = set_status(current_job,true);
        }

        if(res > 0){

            if (current_job -> jobstatus == JOB_RUNNING){
                fprintf(stderr, "[%i] %i Running        %s\n", current_job->id, current_job->pgid, current_job->cmd);
            }
            else if (current_job -> jobstatus == JOB_STOPPED){
                fprintf(stderr, "[%i] %i Stopped        %s\n", current_job->id, current_job->pgid, current_job->cmd);
            }
            else if (current_job -> jobstatus == JOB_DONE){
                fprintf(stderr, "[%i] %i Done        %s\n", current_job->id, current_job->pgid, current_job->cmd);
                suppresion_job(i);
                i--;
            } 
            else if (current_job -> jobstatus == JOB_KILLED){
                fprintf(stderr, "[%i] %i Killed        %s\n", current_job->id, current_job->pgid, current_job->cmd);
                suppresion_job(i);
                i--;
            }
            else{
                fprintf(stderr, "[%i] %i Detached        %s\n", current_job->id, current_job->pgid, current_job->cmd);
                suppresion_job(i);
                i--;
            }
        }
    }
}


int jobs(){
    job *current_job;

    for (int i = 0; i < cmp_jobs; i++){
        current_job = jobs_suivis[i];
        
        set_status(current_job, true);
        if (current_job -> jobstatus == JOB_RUNNING){
            printf("[%i] %i Running        %s\n", current_job->id, current_job->pgid, current_job->cmd);
        }
        else if (current_job -> jobstatus == JOB_DONE){
            printf("[%i] %i Done        %s\n", current_job->id, current_job->pgid, current_job->cmd);
            suppresion_job(i);
            i--;
        }
        else if (current_job -> jobstatus == JOB_STOPPED){
            printf("[%i] %i Stopped        %s\n", current_job->id, current_job->pgid, current_job->cmd);
        }
        else if (current_job -> jobstatus == JOB_KILLED){
            printf("[%i] %i Killed        %s\n", current_job->id, current_job->pgid, current_job->cmd);
            suppresion_job(i);
            i--;
        } 
        else{
            printf("[%i] %i Detached        %s\n", current_job->id, current_job->pgid, current_job->cmd);
            suppresion_job(i);
            i--;
        }           
    }
    return 0;
}