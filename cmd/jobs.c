#include "../lib/env.h"
#include "../lib/jobs.h"
#include "../lib/parseur.h"
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void add_proc_to_job(pid_t pid, pid_t pgid){
    for (int i = 0; i < cmp_jobs; i++){
        job *j = jobs_suivis[i];
        if(j->pgid == pgid){
            j->nb_proc ++;
            j->pid_proc = realloc(j->pid_proc, j->nb_proc*sizeof(pid_t)); 
            j->pid_proc[j->nb_proc-1] = pid;
            return;
        }
    }
    //TODO : erreur
    perror("groupe de job inexistant");
}


job *new_job(pid_t pid, pid_t pgid, char *cmd){
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
    res->pgid = pgid;
    res->nb_proc = 1;
    res->pid_proc = malloc(sizeof(pid_t));
    res->pid_proc[0] = pid;
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
    free(current_job);
    for (int j = i; j < cmp_jobs - 1; j++){
        jobs_suivis[j] = jobs_suivis[j + 1];
    }
    cmp_jobs--;
}

/**si le statut n'a pas changé, renvoie 0, sinon renvoie 1*/
int set_status(job * j, bool bg){
    int info_wait;
    int status;
    int pid;
    bool status_changed = false;
    
    bool running = true;
    bool done = true;
    bool stopped = true;
    bool killed = false;

    for(int i = 0; i < j->nb_proc; i++){
        pid = j->pid_proc[i];
        
        if(bg){
            info_wait = waitpid(pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
        }else{
            info_wait = waitpid(pid, &status, WUNTRACED);
        }
        
        
        if (info_wait == -1){
            perror("waitpid (set_status)");
            return WEXITSTATUS(status);
        }
        if(info_wait == 0){
            continue;
        }
        status_changed = true;

        if (WIFCONTINUED(status)){ //running
            done = false;
            stopped = false;
        }
        else if (WIFEXITED(status)){ //done
            running = false;
        }
        else if (WIFSTOPPED(status)){ //stopped
            running = false;
            done = false;
        }
        else if (WIFSIGNALED(status)){ //killed
            killed = true;
        }
        else{ //detached
            running = false;
            done = false;
            stopped = false;
        }
    }
    
    if(running){
        j->jobstatus = JOB_RUNNING;
    }else if(done){
        j->jobstatus = JOB_DONE;
    }else if(stopped){
        j->jobstatus = JOB_STOPPED;
    }else if(killed){
        j->jobstatus = JOB_KILLED;
    }else{
        j->jobstatus = JOB_DETACHED;
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

        
        if(set_status(current_job, true) > 0){
            //TODO
            if (current_job -> afficher_save) {
                current_job -> afficher_save = false;
            }
            
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
        
        if(set_status(current_job, true) > 0){
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
    }
    return 0;
}