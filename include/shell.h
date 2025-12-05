#ifndef SHELL_H
#define SHELL_H

// --- Standard Libraries ---
// We include them here so we don't have to write them in every .c file
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>   // For file operations
#include <signal.h>  // For signal handling

// --- Constants ---
#define MAX_LINE 256
#define MAX_ARGS 64

// --- Builtin Command Structures ---

// 1. Define the function pointer type
// It accepts argv (char**) and returns int (status, usually 0 or 1)
typedef int (*builtin_func)(char **args);

// 2. Define the struct for a command registry
typedef struct {
    char *name;
    builtin_func func;
} BuiltinCmd;

// --- Function Prototypes ---

// Dispatcher: Returns 1 if a builtin was found and run, 0 otherwise
int exec_builtin(char **args);

// Feature Functions (Public interfaces matching the builtin_func signature)
int set_env(char **args);    // Handles 'set' command
int shell_exit(char **args); // Handles 'exit' command
int get_env(char **args);    // Handles 'get' command
void setup_signals();

#endif
