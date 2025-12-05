#include "../include/shell.h"

int main() {
    char line[MAX_LINE];
    char *argv[MAX_ARGS];

    setup_signals();

    while (1) {
        printf("thinsh> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        
        // ... (Tokenize line into argv as before) ...
        line[strcspn(line, "\n")] = 0;
        int i = 0;
        argv[i] = strtok(line, " ");
        while (argv[i]) argv[++i] = strtok(NULL, " ");

        if (argv[0] == NULL) continue;

        // --- NEW LOGIC ---
        // 1. Try to run as builtin
        if (exec_builtin(argv)) {
            continue; // It was a builtin, loop again
        }

        // 2. Not a builtin? Fork and Exec
        job_exec(argv); 
    }
    return 0;
}
