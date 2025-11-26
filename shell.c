#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char line[256], *argv[64];
    while (printf("thinsh> "), fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0; //  strip \n
        int i = 0;
        argv[i] = strtok(line, " ");
        while (argv[i]) argv[++i] = strtok(NULL, " ");
        
        if (fork() == 0) { // Child
            execvp(argv[0], argv);
            _exit(1); // Kill child if exec fails
        }
        wait(NULL); // Parent waits
    }
    return 0;
}
