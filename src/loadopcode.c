#include "../include/interpreter.h"
#include "../include/shell.h"

const char *opcode_names[] = {"CONG",      "TRU",     "NHAN", "CHIA", "BANG",
                                "LON",       "NHO",     "NHAY", "NNS",  "CAT",
                                "LAY",       "NAP",     "BO",   "IN",   "DUNG",
                                "VAO_TRANG", "RA_TRANG"};
const int opcode_count = sizeof(opcode_names) / sizeof(opcode_names[0]);

int get_opcode(const char *name) {
  for (int i = 0; i < opcode_count; i++) {
    if (strcmp(name, opcode_names[i]) == 0) {
      return i;
    }
  }
  return -1;
}
void load_program(CHUONG_TRINH *program, const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    perror("Không thể mở file");
    exit(1);
  }
  char line[256];
  int line_count = 0;
  while (fgets(line, sizeof(line), file)) {
    line_count++;
    line[strcspn(line, "\n")] = 0;
    char *token = strtok(line, " \t");
    if (!token || token[0] == '#') continue;
    
    int opcode = get_opcode(token);
    if (opcode == -1) {
      fprintf(stderr, "Lỗi tại dòng %d: Opcode không hợp lệ '%s'\n", line_count,
              token);
      exit(1);
    }
    char *operand_str = strtok(NULL, " \t");
    int operand = 0;
    if (operand_str) {
      operand = atoi(operand_str);
    } // tách tại tab và dấu cách, lấy phần tử đầu tiên là opcode, phần tử thứ
      // hai là operand
    if (opcode == NAP) {
      add_constant(program, operand);
      operand = program->constants_size - 1;
    }
    //case đặc biệt, nếu là NAP thì người dùng nhập thẳng giá trị hằng số, không quan tâm index
    add_instruction(program, opcode, operand);
  }
  fclose(file);
}
