#include "../include/shell.h"
#include <readline/history.h>

int main() {
    char *input; 
    char *argv[MAX_ARGS];

    setup_signals();

    while (1) {
        input = readline("thinsh> ");
        if (input == NULL) break;
        
        // ... (Tokenize line into argv as before) ...
        input[strcspn(input, "\n")] = 0;
        int i = 0;
        add_history(input);
        argv[i] = strtok(input, " ");
        while (argv[i]) argv[++i] = strtok(NULL, " ");

        if (argv[0] == NULL) continue;

        // --- NEW LOGIC ---
        // 1. Try to run as builtin
        if (exec_builtin(argv)) {
            continue; // It was a builtin, loop again
        }

        // 2. Not a builtin? Fork and Exec
        job_exec(argv); 

        free(input);
    }
    return 0;
}
