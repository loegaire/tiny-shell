#include "../include/shell.h"

int main() {
    char *input; 
    char *argv[MAX_ARGS];

    #ifdef USE_READLINE
    rl_catch_signals = 0; 
    #endif

    setup_signals();

    while (1) {
        #ifdef USE_READLINE
        input = readline("thinsh> ");
        if (input && *input) add_history(input);
        #else
        char buf[MAX_LINE];
        fputs("thinsh> ", stdout);
        fflush(stdout);
        if (!fgets(buf, sizeof(buf), stdin)) {
            input = NULL;
        } else {
            input = strdup(buf);
        }
        #endif
        if (input == NULL) break;
        
        input[strcspn(input, "\n")] = 0;
        int i = 0;
        #ifdef USE_READLINE
        #endif
        argv[i] = strtok(input, " ");
        while (argv[i]) argv[++i] = strtok(NULL, " ");

        if (argv[0] == NULL) { free(input); continue; }

        if (exec_builtin(argv)) {
            free(input);
            continue; 
        }
        signal(SIGTSTP, SIG_IGN);

        job_exec(argv); 

        free(input);
    }
    return 0;
}
