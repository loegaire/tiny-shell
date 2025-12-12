#include "../include/interpreter.h"
#include "../include/shell.h"
void init_CHUONG_TRINH(CHUONG_TRINH *program) {
  program->code = NULL;
  program->code_size = 0;
  program->code_capacity = 0;
  program->constants = NULL;
  program->constants_size = 0;
  program->constants_capacity = 0;
}
void init_MAY_AO(MAY_AO *vm, CHUONG_TRINH *program) {
  vm->program = program;
  vm->pc = 0;
  vm->sp = 0;
  vm->is_in_trang = 0;
  vm->backup_sp = 0;
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
  int value = pop(vm);
  printf("%d\n", value);
}
void OP_VAO_TRANG(MAY_AO *vm) {
  if (vm->is_in_trang ) {
    printf("Loi: Khong ho tro 'trang' long nhau!\n");
    return;
  }
  vm->is_in_trang = 1;
  vm->backup_sp = vm->sp;
  for (int i = 0; i < 1024; i++) {
    vm->backup_vars[i] = vm->vars[i];
  }
}
void OP_RA_TRANG(MAY_AO *vm) {
  vm->is_in_trang = 0;
  // Phục hồi trạng thái stack và biến từ trang trước
  vm->sp = vm->backup_sp;
  for (int i = 0; i < 1024; i++) {
    vm->vars[i] = vm->backup_vars[i];
  }
}
void run(MAY_AO *vm) {
  while (vm->pc < vm->program->code_size) {
    CHI_DAN instruction = vm->program->code[vm->pc];
    vm->pc++; // Tăng bộ đếm chương trình trước khi thực thi
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
      OP_IN(vm);
      break;
    case DUNG:
      return;
      break;
    case VAO_TRANG:
      OP_VAO_TRANG(vm);
      break;
    case RA_TRANG:
      OP_RA_TRANG(vm);
      break;
    default:
      printf("Loi: Lenh khong hop le!\n");
      exit(1);
    }
  }
}
int main() {
    printf("--- KHOI TAO MAY AO ---\n");

    // 1. Khởi tạo chương trình
    CHUONG_TRINH prog;
    init_CHUONG_TRINH(&prog);

    // 2. Load code từ file "code.txt"
    // Lưu ý: Đảm bảo file code.txt nằm cùng thư mục khi chạy
    load_program(&prog, "code.txt");
    printf("-> Da load %d lenh.\n", prog.code_size);

    // 3. Khởi tạo VM và nạp chương trình vào
    MAY_AO vm;
    init_MAY_AO(&vm, &prog);

    // 4. Chạy
    printf("--- KET QUA CHAY ---\n");
    run(&vm);
    
    printf("--- KET THUC ---\n");

    // 5. Dọn dẹp bộ nhớ (tốt cho thói quen)
    if (prog.code) free(prog.code);
    if (prog.constants) free(prog.constants);

    return 0;
}
