#include "../include/shell.h" // Use quotes for local headers

int main() {
    char line[MAX_LINE];
    char *argv[MAX_ARGS];
    while (1) {
        printf("thinsh>> ");
        
        // 1. READ
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n"); // Handle Ctrl+D (EOF) gracefully
            break; 
        }

        // Remove newline character
        line[strcspn(line, "\n")] = 0;

        // Skip empty lines
        if (strlen(line) == 0) continue;

        // 2. PARSE (Simple tokenization for now)
        int i = 0;
        argv[i] = strtok(line, " ");
        while (argv[i] && i < MAX_ARGS - 1) {
            argv[++i] = strtok(NULL, " ");
        }

        // Handle "exit" command directly (Basic Built-in)
        if (argv[0] && strcmp(argv[0], "exit") == 0) {
            break;
        }

        // 3. EXECUTE
        if (argv[0]) {
            pid_t pid = fork();

            if (pid == 0) { 
                // Child Process
                if (execvp(argv[0], argv) == -1) {
                    perror("Execution failed");
                }
                exit(1);
            } else if (pid > 0) {
                // Parent Process
                wait(NULL);
            } else {
                perror("Fork failed");
            }
        }
    }

    return 0;
}
