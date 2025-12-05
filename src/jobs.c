#include "../include/shell.h"

int job_exec(char **args){
    int bg=0;
    for(int i=0;i<MAX_ARGS;i++){
        if(args[i]=='&'){
            bg=1;
            args[i]=NULL;
        }
    }

    return 0;
}
