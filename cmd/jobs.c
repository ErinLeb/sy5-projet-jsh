#include "../lib/env.h"
#include "../lib/jobs.h"
#include "../lib/parseur.h"
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


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

void print_child(int target_pid,int space) {
    char children_path[1024];
    snprintf(children_path, sizeof(children_path), "/proc/%d/task/%d/children", target_pid, target_pid);
    int children_file = open(children_path, O_RDONLY);

    if (children_file != -1) {
        int MAX_SIZE_PID = 1024;
        int MAX_CHILD_JOBS = 1024;
        char buf[MAX_CHILD_JOBS*MAX_SIZE_PID];
        int* pid_fils = NULL;
        read(children_file,buf,MAX_CHILD_JOBS*MAX_SIZE_PID);
        char *pid = strtok(buf, " ");
        int nombre_fils = 0;

        while (pid != NULL) {
            pid_fils = realloc(pid_fils, (nombre_fils + 1) * sizeof(int));
            pid_fils[nombre_fils] = atoi(pid);
            nombre_fils++;

            pid = strtok(NULL, " ");
        }

        for (int i = 0 ; i < nombre_fils; i++) {
            int child_pid = pid_fils[i];

            char info_path[512];
            snprintf(info_path, sizeof(info_path), "/proc/%d/status", child_pid);
            
            FILE * info = fopen(info_path, "r");

            if (info == NULL) {
                perror("open fail");
            }

            char prog_name[1024];
            bool prog_name_find = false;
            char prog_state[1024];
            bool prog_state_find = false;

            char line[1024];
            while (fgets(line, sizeof(line), info) != NULL) {
                if (strncmp(line,"State:	",7) == 0){
                    strcpy(prog_state, line+7);
                    strcat(prog_state,"\0");
                    prog_state_find = true;
                }
                if (strncmp(line,"Name:	",6) == 0){
                    strcpy(prog_name, line+6);
                    strcat(prog_name,"\0");
                    prog_name_find = true;
                }
                if (prog_name_find && prog_state_find){
                    break;
                }
            }
            fclose(info);

            if (! prog_name_find || ! prog_state_find){
                perror("error prog_name_find or prog_stat_find");
            }

            for (int i = 0; i < space; ++i) {
                printf("  |");
            }

            if (strcmp(prog_state,"R") == 0 || strcmp(prog_state,"S") == 0 || strcmp(prog_state,"D") == 0){
                printf("%d Running        %s", child_pid, prog_name);
            }
            else if (strcmp(prog_state,"T")){
                printf("%d Stopped        %s", child_pid, prog_name);
            }
            else if (strcmp(prog_state,"Z") == 0){
                printf("%d Detached        %s", child_pid, prog_name);
            }else{
                printf("%d Unknown        %s", child_pid, prog_name);
            }

            print_child(child_pid, space + 1);
        }
        close(children_file);
    }
    else {
        perror("open children_file");
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


int jobs_t(){

    job *current_job;

    for (int i = 0; i < cmp_jobs; i++){
        current_job = jobs_suivis[i];
        for (int j = 0; j < current_job->nb_proc; j++){
            if (current_job -> status_proc[j] == JOB_RUNNING){
                printf("[%i] %i Running        %s\n", current_job->id, current_job->pid_proc[j], current_job->cmd_proc[j]);
            }
            else if (current_job -> status_proc[j] == JOB_DONE){
                printf("[%i] %i Done        %s\n",current_job->id, current_job->pid_proc[j], current_job->cmd_proc[j]);
                suppresion_job(i);
                i--;
            }
            else if (current_job -> status_proc[j] == JOB_STOPPED){
                printf("[%i] %i Stopped        %s\n", current_job->id, current_job->pid_proc[j], current_job->cmd_proc[j]);
            }
            else if (current_job -> status_proc[j] == JOB_KILLED){
                printf("[%i] %i Killed        %s\n", current_job->id, current_job->pid_proc[j], current_job->cmd_proc[j]);
                suppresion_job(i);
                i--;
            } 
            else{
                printf("[%i] %i Detached        %s\n", current_job->id, current_job->pid_proc[j], current_job->cmd_proc[j]);
                suppresion_job(i);
                i--;
            } 
            print_child(current_job ->pid_proc[j] ,1);     
        }  
    }
    return 0;
}
