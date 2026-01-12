#include "../include/shell.h"

int main() {
    char *input; 
    char *argv[MAX_ARGS];

    setup_signals();

    while (1) {
        #ifdef USE_READLINE
        input = readline("thinsh> ");
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
        
        // ... (Tokenize line into argv as before) ...
        input[strcspn(input, "\n")] = 0;
        int i = 0;
        #ifdef USE_READLINE
        add_history(input);
        #endif
        argv[i] = strtok(input, " ");
        while (argv[i]) argv[++i] = strtok(NULL, " ");

        if (argv[0] == NULL) { free(input); continue; }

        // --- NEW LOGIC ---
        // 1. Try to run as builtin
        if (exec_builtin(argv)) {
            free(input);
            continue; // It was a builtin, loop again
        }

        // 2. Not a builtin? Fork and Exec
        job_exec(argv); 

        free(input);
    }
    return 0;
}
