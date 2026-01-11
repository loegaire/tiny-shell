#include "../include/shell.h"

// --- 1. Internal Job Storage (Hidden from other files) ---
typedef struct {
    int pid;
    int id;
    int status; // 1 = Running, 2 = Stopped
    char cmd[256];
} Job;

#define MAX_JOBS 64
Job job_list[MAX_JOBS];
int job_count = 0;

// Helper to add a job
void add_job(int pid, char *name) {
    if (job_count < MAX_JOBS) {
        job_list[job_count].pid = pid;
        job_list[job_count].id = job_count + 1;
        job_list[job_count].status = 2; // Stopped
        strncpy(job_list[job_count].cmd, name, 255);
        job_count++;
    }
}

// Helper to find a job by ID
int find_job_index(int job_id) {
    for (int i = 0; i < job_count; i++) {
        if (job_list[i].id == job_id) return i;
    }
    return -1;
}

int job_exec(char **args){
    // --- 2. Intercept the 'fg' command ---
    // Since 'fg' isn't in builtins.c, main.c sends it here.
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

        // A. Send SIGCONT to the process
        if (kill(pid, SIGCONT) < 0) {
            perror("Failed to send SIGCONT");
            return 1;
        }

        // B. Wait for it again (Foregound behavior)
        int status;
        waitpid(pid, &status, WUNTRACED);

        // C. Handle the result
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            // Job finished, remove it (simple swap with last element to delete)
            job_list[idx] = job_list[job_count - 1];
            job_count--;
        } 
        else if (WIFSTOPPED(status)) {
            printf("\nProcess %d stopped again.\n", pid);
            // It remains in the list, no change needed
        }
        
        return 0; // Success
    }

    // --- Standard Execution Logic ---
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
        // 1. Create a new Process Group
        // This isolates the child so it can be controlled independently.
        setpgid(0, 0);            // 2. Reset Signal Handlers to Default
        // We want the child to STOP on SIGTSTP, not ignore it like the shell.
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);            
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        // 3. UNBLOCK Signals (Crucial!)
        // If the signal is masked, the OS will never deliver the Stop signal.
        sigset_t sigs;            
        sigemptyset(&sigs);
        sigaddset(&sigs, SIGTSTP);
        sigprocmask(SIG_UNBLOCK, &sigs, NULL);        
        // 4. Execute the command
        execvp(args[0], args);
        perror("Execution failed"); 
        // Print error if execvp fails            
        exit(1);
    } 
    else {
        // --- PARENT PROCESS ---
        int status;
        if (bg == 0) {
            // Wait for the child (now in its own group)
            waitpid(pid, &status, WUNTRACED);

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
