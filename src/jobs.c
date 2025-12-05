#include "../include/shell.h"
#include <stdlib.h>

int job_exec(char **args){
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
        perror("Fork không thành công (hết memory?)");
        return -1;
    }
    if(pid==0){
        execvp(args[0], args);
        exit(1);
    }
    else{
        int status;
        if(bg==0){
            waitpid(pid, &status, WUNTRACED);
            if(WIFSTOPPED(status)){
                printf("\nProcess %d stopped\n",pid);
            }
        }
        else{
            printf("%d",pid);
        }
    }

    return 0;
}
