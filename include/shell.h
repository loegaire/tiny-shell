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
#include <fcntl.h>   // For file operations later
#include <signal.h>  // For signal handling later

// --- Constants ---
#define MAX_LINE 256
#define MAX_ARGS 64

// --- Future Function Prototypes ---
// As you add features, you will declare functions here.
// For example, later you will add:
// void setup_signals();
// void add_job(pid_t pid);

#endif
