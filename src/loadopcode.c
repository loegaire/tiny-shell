#include "../include/interpreter.h"
#include "../include/shell.h"

const char *opcode_names[20] = {"CONG",      "TRU",     "NHAN", "CHIA", "BANG",
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
    if (line[0] == '#' || strlen(line) == 0) {
      continue; // Bỏ qua dòng chú thích và dòng trống
    }
    char *token = strtok(line, " \t");
    if (!token)
      continue;
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
    if (program->code_size >= program->code_capacity) {
      program->code_capacity *= 2;
      program->code =
          realloc(program->code, program->code_capacity * sizeof(CHI_DAN));
    }
    add_instruction(program, opcode, operand);    
  }
  fclose(file);
}
