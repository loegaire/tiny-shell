#include "../include/shell.h"

typedef struct {
    int pid;
    int id;
    int status; 
    char cmd[256];
} Job;

#define MAX_JOBS 64
Job job_list[MAX_JOBS];
int job_count = 0;

void add_job(int pid, char *name) {
    if (job_count < MAX_JOBS) {
        job_list[job_count].pid = pid;
        job_list[job_count].id = job_count + 1;
        job_list[job_count].status = 2; // Stopped
        strncpy(job_list[job_count].cmd, name, 255);
        job_count++;
    }
}

int find_job_index(int job_id) {
    for (int i = 0; i < job_count; i++) {
        if (job_list[i].id == job_id) return i;
    }
    return -1;
}

int job_exec(char **args){
    if (strcmp(args[0], "fg") == 0) {
        if (args[1] == NULL) {
            printf("Usage: fg <job_id>\n");
            return 1;
        }
        
        int job_id = atoi(args[1]);
        int idx = find_job_index(job_id);

        if (idx == -1) {
            printf("Job %d not found.\n", job_id);
            return 1;
        }

        int pid = job_list[idx].pid;
        printf("Resuming job %d: %s\n", pid, job_list[idx].cmd);

        tcsetpgrp(STDIN_FILENO, pid);

        if (kill(pid, SIGCONT) < 0) {
            perror("Failed to send SIGCONT");
            return 1;
        }

        int status;
        waitpid(pid, &status, WUNTRACED);
        tcsetpgrp(STDIN_FILENO, getpgrp());

        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            job_list[idx] = job_list[job_count - 1];
            job_count--;
        } 
        else if (WIFSTOPPED(status)) {
            printf("\nProcess %d stopped again.\n", pid);
        }
        
        return 0; 
    }

    int bg=0;
    for(int i=0;args[i]!=NULL;i++){
        if(strcmp(args[i],"&")==0){
            bg=1;
            args[i]=NULL;
        }
    }
    if(args[0]==NULL) return 1;

    int pid=fork();
    if(pid<0){
        perror("Fork không thành công");
        return -1;
    }
    
    if(pid==0){
        setpgid(0, 0);
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);            
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        sigset_t sigs;            
        sigemptyset(&sigs);
        sigaddset(&sigs, SIGTSTP);
        sigprocmask(SIG_UNBLOCK, &sigs, NULL);        
        execvp(args[0], args);
        perror("Execution failed"); 
        exit(1);
    } 
    else {
        setpgid(pid, pid);
        if (bg == 0) {
            tcsetpgrp(STDIN_FILENO, pid);
            int status;
            waitpid(pid, &status, WUNTRACED);
            tcsetpgrp(STDIN_FILENO, getpgrp());
            if (WIFSTOPPED(status)) {
                printf("\nProcess %d stopped. Job ID: %d\n", pid, job_count + 1);
                add_job(pid, args[0]); 
            }
        } else {
            printf("Started background process %d\n", pid);
        }
    }

    return 0;
}
