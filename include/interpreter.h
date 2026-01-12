#include <stdint.h>
#include <ctype.h>
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
  DUNG,
  VAO_TRANG,
  RA_TRANG
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

  // NEW: command table for "run external command" statements (e.g., `date`)
  char **cmds;
  int cmds_size;
  int cmds_capacity;
} CHUONG_TRINH;

typedef struct {
  CHUONG_TRINH *program;
  int pc;
  int stack[1024];
  int sp;
  int backup_stack[1024];
  int backup_sp;
  int vars[1024];
  int backup_vars[1024];
  int is_in_trang;
} MAY_AO;
void init_CHUONG_TRINH(CHUONG_TRINH *program);
void init_MAY_AO(MAY_AO *vm, CHUONG_TRINH *program);
void add_instruction(CHUONG_TRINH *program, uint8_t opcode, int operand);
void add_constant(CHUONG_TRINH *program, int value);
int get_constant(CHUONG_TRINH *program, int index);
void run(MAY_AO *vm);
void load_program(CHUONG_TRINH *program, const char *filename) ;

// Trang compiler (implemented in src/compiler.c)
void compile(CHUONG_TRINH *program, const char *source);

// NEW: add a command string to program->cmds and return its index
int add_command(CHUONG_TRINH *program, const char *start, int length);
