#include "../include/shell.h"

// --- Wrappers ---

// We still need a wrapper for exit because the standard exit() 
// takes an int, but our table expects a function taking (char**)
int builtin_exit(char **args) {
    exit(0);
    return 0; // Unreachable, but keeps compiler happy
}

// --- THE LOOKUP TABLE ---
// Register new commands here!
BuiltinCmd builtins[] = {
    // NAME    FUNCTION
    // We can use set_env directly because it now fits the "int func(char**)" shape!
    {"set",  set_env},      
    {"exit", builtin_exit},
    {NULL, NULL} // Terminator
};

// --- THE DISPATCHER ---
// Main calls this. It loops through the table to find a match.
int exec_builtin(char **args) {
    if (args[0] == NULL) return 0;

    for (int i = 0; builtins[i].name != NULL; i++) {
        if (strcmp(args[0], builtins[i].name) == 0) {
            // Found a match! Run the function.
            return builtins[i].func(args);
        }
    }
    return 0; // Not a builtin, let main fork()
}
