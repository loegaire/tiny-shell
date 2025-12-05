#include "../include/shell.h"

int set_env(char **args) {
    int is_ok = EXIT_FAILURE;
    if (args[1] == NULL || args[2] == NULL) {
        fprintf(stderr, "cach dung: set <key> <value>\n");
        return is_ok;
    }
    char *name = args[1];
    char *value = args[2];
    FILE* fp = fopen("env.txt", "a");
    if (!fp) {
        perror("khong mo duoc file!:<");
        return is_ok;
    }
    fprintf(fp, "%s=%s\n", name, value);
    fclose(fp);
    return EXIT_SUCCESS;
}
