#include "../include/interpreter.h"
#include "../include/shell.h"
#include <string.h>
#include <stdlib.h>

void init_CHUONG_TRINH(CHUONG_TRINH *program) {
  program->code = NULL;
  program->code_size = 0;
  program->code_capacity = 0;
  program->constants = NULL;
  program->constants_size = 0;
  program->constants_capacity = 0;

  // NEW
  program->cmds = NULL;
  program->cmds_size = 0;
  program->cmds_capacity = 0;
}

// NEW
int add_command(CHUONG_TRINH *program, const char *start, int length) {
  if (length <= 0) return -1;

  if (program->cmds_size >= program->cmds_capacity) {
    program->cmds_capacity = program->cmds_capacity == 0 ? 4 : program->cmds_capacity * 2;
    program->cmds = realloc(program->cmds, program->cmds_capacity * sizeof(char *));
    if (!program->cmds) {
      perror("add_command: realloc");
      exit(1);
    }
  }

  char *s = (char *)malloc((size_t)length + 1);
  if (!s) {
    perror("add_command: malloc");
    exit(1);
  }
  memcpy(s, start, (size_t)length);
  s[length] = '\0';

  program->cmds[program->cmds_size] = s;
  return program->cmds_size++;
}

void init_MAY_AO(MAY_AO *vm, CHUONG_TRINH *program) {
  vm->program = program;
  vm->pc = 0;
  vm->sp = 0;
  vm->is_in_trang = 0;
  vm->backup_sp = 0;

  // Ensure deterministic state: undefined vars become 0
  memset(vm->stack, 0, sizeof(vm->stack));
  memset(vm->backup_stack, 0, sizeof(vm->backup_stack));
  memset(vm->vars, 0, sizeof(vm->vars));
  memset(vm->backup_vars, 0, sizeof(vm->backup_vars));
}

void push(MAY_AO *vm, int value) {
  if (vm->sp >= 1024) {
    printf("Loi nghiem trong: Tran ngan xep (Stack Overflow)!\n");
    exit(1); // Dừng chương trình ngay lập tức
  }
  vm->stack[vm->sp++] = value;
}

int pop(MAY_AO *vm) {
  if (vm->sp <= 0) {
    printf("Loi nghiem trong: Ngan xep rong (Stack Underflow)!\n");
    exit(1); // Dừng chương trình ngay lập tức
  }
  vm->sp--;
  return vm->stack[vm->sp];
} // Tư duy: tạo máy áo, bộ chỉ dẫn, cấu trúc câu lệnh và chương trình
void add_instruction(CHUONG_TRINH *program, uint8_t opcode, int operand) {
  if (program->code_size >= program->code_capacity) {
    // nếu đầy thì cấp phát thêm gấp đôi
    program->code_capacity =
        program->code_capacity == 0 ? 4 : program->code_capacity * 2;
    program->code =
        realloc(program->code, program->code_capacity * sizeof(CHI_DAN));
  }
  program->code[program->code_size].opcode = opcode;
  program->code[program->code_size].operand = operand;
  program->code_size++;
}
void add_constant(CHUONG_TRINH *program, int value) {
  if (program->constants_size >= program->constants_capacity) {
    program->constants_capacity =
        program->constants_capacity == 0 ? 4 : program->constants_capacity * 2;
    program->constants =
        realloc(program->constants, program->constants_capacity * sizeof(int));
  }
  program->constants[program->constants_size++] = value;
}
int get_constant(CHUONG_TRINH *program, int index) {
  if (index < 0 || index >= program->constants_size) {
    printf("Loi: Chi so hang so khong hop le!\n");
    exit(1);
  }
  return program->constants[index];
}
void OP_CONG(MAY_AO *vm) {
  int a = pop(vm);
  int b = pop(vm);
  push(vm, a + b);
}
void OP_TRU(MAY_AO *vm) {
  int a = pop(vm);
  int b = pop(vm);
  push(vm, b - a);
}
void OP_NHAN(MAY_AO *vm) {
  int a = pop(vm);
  int b = pop(vm);
  push(vm, a * b);
}
void OP_CHIA(MAY_AO *vm) {
  int a = pop(vm);
  int b = pop(vm);
  if (a == 0) {
    printf("Loi: Chia cho 0!\n");
    exit(1);
  }
  push(vm, b / a);
}
void OP_BANG(MAY_AO *vm) {
  int a = pop(vm);
  int b = pop(vm);
  push(vm, b == a);
}
void OP_LON(MAY_AO *vm) {
  int a = pop(vm);
  int b = pop(vm);
  push(vm, b > a);
}
void OP_NHO(MAY_AO *vm) {
  int a = pop(vm);
  int b = pop(vm);
  push(vm, b < a);
}
void OP_NHAY(MAY_AO *vm, int operand) { vm->pc = operand; }

void OP_NNS(MAY_AO *vm, int operand) {
  int condition = pop(vm);
  if (!condition) { // Nếu sai (0) thì nhảy
    vm->pc = operand;
  }
}
// thao tác với biến
void OP_CAT(MAY_AO *vm, int operand) {
  int value = pop(vm);
  vm->vars[operand] = value;
}
void OP_LAY(MAY_AO *vm, int operand) {
  int value = vm->vars[operand];
  push(vm, value);
}
// thao tác lên stack
void OP_NAP(MAY_AO *vm, int index) {
  int value = get_constant(vm->program, index);
  push(vm, value);
}
void OP_BO(MAY_AO *vm, int operand) {
  pop(vm);
}
void OP_IN(MAY_AO *vm) {
  // Deprecated: printing is handled by OP_IN_WITH_OPERAND(vm, operand).
  // Kept to avoid breaking old references.
  int value = pop(vm);
  printf("%d\n", value);
}

// REPLACE OP_IN with an operand-aware version
void OP_IN_WITH_OPERAND(MAY_AO *vm, int operand) {
  if (operand == 0) {
    int value = pop(vm);
    printf("%d\n", value);
    return;
  }

  if (operand == 1) {
    int cmdIndex = pop(vm);
    if (cmdIndex < 0 || cmdIndex >= vm->program->cmds_size) {
      printf("Loi: cmdIndex khong hop le!\n");
      exit(1);
    }

    // Execute like the shell does: argv[0] only (no args for now).
    char *argv[2];
    argv[0] = vm->program->cmds[cmdIndex];
    argv[1] = NULL;

    job_exec(argv);
    return;
  }

  printf("Loi: IN operand khong hop le!\n");
  exit(1);
}

void OP_VAO_TRANG(MAY_AO *vm) {
  if (vm->is_in_trang) {
    printf("Loi: Khong ho tro 'trang' long nhau!\n");
    exit(1);
  }
  vm->is_in_trang = 1;

  // Backup outer state (stack content up to sp + all vars)
  vm->backup_sp = vm->sp;
  for (int i = 0; i < vm->backup_sp; i++) {
    vm->backup_stack[i] = vm->stack[i];
  }
  for (int i = 0; i < 1024; i++) {
    vm->backup_vars[i] = vm->vars[i];
  }

  // Fresh sandbox state
  vm->sp = 0;
  for (int i = 0; i < 1024; i++) {
    vm->vars[i] = 0;
  }
}

void OP_RA_TRANG(MAY_AO *vm) {
  if (!vm->is_in_trang) {
    // Ignore or treat as error; error is safer.
    printf("Loi: 'ra_trang' khi khong o trong 'trang'!\n");
    exit(1);
  }

  // "Return useful results": if sandbox left something on stack,
  // bring back the TOP value only.
  int has_result = (vm->sp > 0);
  int result = 0;
  if (has_result) {
    result = vm->stack[vm->sp - 1];
  }

  // Restore outer state
  vm->sp = vm->backup_sp;
  for (int i = 0; i < vm->backup_sp; i++) {
    vm->stack[i] = vm->backup_stack[i];
  }
  for (int i = 0; i < 1024; i++) {
    vm->vars[i] = vm->backup_vars[i];
  }

  vm->is_in_trang = 0;

  // Push returned result onto outer stack (default 0).
  push(vm, result);
}
void run(MAY_AO *vm) {
  while (vm->pc < vm->program->code_size) {
    CHI_DAN instruction = vm->program->code[vm->pc];
    vm->pc++;
    switch (instruction.opcode) {
      case CONG:
        OP_CONG(vm);
        break;
      case TRU:
        OP_TRU(vm);
        break;
      case NHAN:
        OP_NHAN(vm);
        break;
      case CHIA:
        OP_CHIA(vm);
        break;
      case BANG:
        OP_BANG(vm);
        break;
      case LON:
        OP_LON(vm);
        break;
      case NHO:
        OP_NHO(vm);
        break;
      case NHAY:
        OP_NHAY(vm, instruction.operand);
        break;
      case NNS:
        OP_NNS(vm, instruction.operand);
        break;
      case CAT:
        OP_CAT(vm, instruction.operand);
        break;
      case LAY:
        OP_LAY(vm, instruction.operand);
        break;
      case NAP:
        OP_NAP(vm, instruction.operand);
        break;
      case BO:
        OP_BO(vm, instruction.operand);
        break;
      case IN:
        OP_IN_WITH_OPERAND(vm, instruction.operand);
        break;
      case DUNG:
        return;
      case VAO_TRANG:
        OP_VAO_TRANG(vm);
        break;
      case RA_TRANG:
        OP_RA_TRANG(vm);
        break;
      default:
        printf("Loi: Lenh khong hop le! opcode=%d operand=%d pc=%d\n",
               (int)instruction.opcode, instruction.operand, vm->pc - 1);
        exit(1);
    }
  }
}

// Standalone test driver: keep it, but don't compile into thinsh by default.
#ifdef TINY_VM_STANDALONE_MAIN
int main() {
    printf("--- KHOI TAO MAY AO ---\n");

    CHUONG_TRINH prog;
    init_CHUONG_TRINH(&prog);

    load_program(&prog, "code.txt");
    printf("-> Da load %d lenh.\n", prog.code_size);

    MAY_AO vm;
    init_MAY_AO(&vm, &prog);

    printf("--- KET QUA CHAY ---\n");
    run(&vm);

    printf("--- KET THUC ---\n");

    if (prog.code) free(prog.code);
    if (prog.constants) free(prog.constants);

    return 0;
}
#endif
