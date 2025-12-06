#include "../include/shell.h"
enum opcode {
  CONG,
  TRU,
  NHAN,
  CHIA,
  BANG,
  LON,
  NHO,
  NHAY,
  NNS,
  CAT,
  LAY,
  NAP,
  BO,
  IN,
  DUNG
};

typedef struct {
  uint8_t opcode;
  int operand;
} CHI_DAN;
typedef struct {
  CHI_DAN *code;
  int code_size;
  int code_capacity;
  int *constants;
  int constants_size;
  int constants_capacity;
} CHUONG_TRINH;

typedef struct {
  CHUONG_TRINH *program;
  int pc;
  int stack[1024];
  int sp;
} MAY_AO;

void init_PROGRAM(CHUONG_TRINH *program) {
  program->code = NULL;
  program->code_size = 0;
  program->code_capacity = 0;
  program->constants = NULL;
  program->constants_size = 0;
  program->constants_capacity = 0;
}
void init_VM(MAY_AO *vm, CHUONG_TRINH *program) {
  vm->program = program;
  vm->pc = 0;
  vm->sp = 0;
}
void push(MAY_AO *vm , int value) {
	vm->stack[vm->sp++] = value;  
}
void pop(MAY_AO *vm) {
	vm->sp--;  
}
