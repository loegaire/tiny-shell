#include "../include/shell.h"

int job_exec(char **args){
    int bg=0;
    for(int i=0;args[i]!=NULL;i++){
        if(strcmp(args[i],"&")==0){
            bg=1;
            args[i]=NULL;
        }
    }
    if(args[0]==NULL) return -1;
    int pid=fork();
    if(pid<0){
        printf("Fork không thành công (hết memory?)");
        return -1;
    }
    if(pid==0){
        execvp(args[0], args);
        exit(1);
    }
    else{
        if(bg==0){
            wait(NULL);
        }
        else{
            printf("%d",pid);
        }
    }

    return 0;
}
