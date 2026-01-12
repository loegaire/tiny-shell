#include "../include/shell.h"

void sig_handler(int sig){
    printf("\n");  
}

void setup_signals(){
    signal(SIGINT, sig_handler);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
}
