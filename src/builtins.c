#include "../include/shell.h"

// --- Wrappers ---

// takes an int, but our table expects a function taking (char**)
int builtin_exit(char **args) {
    exit(0);
    return 0; // Unreachable, but keeps compiler happy
}

int builtin_help(char **args){
    printf("--- thinsh Help ---\n");
    printf("dir/ls    : Xem danh sách các file/directories trong directory hiện tại.\n");
    printf("date      : Xem ngày-tháng-năm-giờ.\n");
    printf("history   : Xem lịch sử các lệnh.\n");
    printf("path      : Xem vị trí directory hiện tại.\n");
    printf("addpath X : Thêm  X vào  PATH.\n");
    printf("exit      : Thoát khỏi thinsh.\n");
    printf("command & : Chạy 1 chương trình trong nền.\n");
    printf("Lưu ý     : Dùng các lệnh có sẵn của Linux (ps, kill,...) để xem danh sách tiến trình, hủy tiến trình,...\n");
    return 1;
}

int builtin_cd(char **args){
    char *target=NULL;
    if(args[1]==NULL){
        target=getenv("HOME");
    } 
    else target=args[1]; 
    chdir(target);
    return 1;
}

// --- THE LOOKUP TABLE ---
// Register new commands here!
BuiltinCmd builtins[] = {
    // NAME    FUNCTION
    {"set",  set_env},      
    {"exit", builtin_exit},
    {"help", builtin_help},
    {"cd", builtin_cd},
    {NULL, NULL} 
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
