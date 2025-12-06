#include <stdint.h>
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
void add_instruction(CHUONG_TRINH *program, uint8_t opcode, int operand);
