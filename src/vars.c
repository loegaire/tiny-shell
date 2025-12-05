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
int get_env(char **args){
    if (args[1] == NULL) {
        fprintf(stderr, "cach dung: get <key>\n");
        return EXIT_FAILURE;
    }
    char *name = args[1];
    FILE* fp = fopen("env.txt", "r");
    if (!fp) {
        perror("khong mo duoc file!:<");
        return EXIT_FAILURE;
    }
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
	char *eq_position = strchr(line, '=');
	if (eq_position) {
	*eq_position = '\0';
	    if (strcmp(line, name) == 0) {
	        printf("%s\n", eq_position + 1);
	        fclose(fp);
		return EXIT_SUCCESS;
	    }
        }
    }
    fclose(fp);
    fprintf(stderr, "Bien %s khong ton tai.\n", name);
    return EXIT_FAILURE;
}
