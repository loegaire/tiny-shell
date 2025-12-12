#include "../include/interpreter.h"
#include "../include/shell.h"

typedef enum {
  TOKEN_NEU,
  TOKEN_THI,
  TOKEN_HET,
  TOKEN_KHI,
  TOKEN_TRANG,
  TOKEN_EQUAL,
  TOKEN_SEMICOLON, // = ;
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH, // + - * /
  TOKEN_LESS,
  TOKEN_GREATER,
  TOKEN_EQUAL_EQUAL, // < > ==
  TOKEN_VAR,
  TOKEN_NUMBER,
  TOKEN_EOF,
  TOKEN_ERROR,
  TOKEN_NEWLINE
} TU_KHOA;

typedef struct {
  const char *start;
  int length;
  int line;
  TU_KHOA type;
} TOKEN;
typedef struct {
  const char *start;   // con trỏ đầu dòng
  const char *current; // con trỏ hiện tại đang chạy
  int current_line;
  CHUONG_TRINH *prog;

} SCANNER;
SCANNER scanner;
void init_scanner(const char *source) {
  scanner.start = source;
  scanner.current = source;
  scanner.current_line = 1;
  scanner.prog = NULL;
}
int is_at_end() { return *scanner.current == '\0'; }
void advance() { scanner.current++; }
void skip_whitespace() {
  for (;;) {
    char c = *scanner.current;
    switch (c) {
    case ' ':
    case '\r':
    case '\t':
      advance();
      break;
    case '#':
      while (*scanner.current != '\n' && !is_at_end())
        advance();
      break;
    default:
      return;
    }
  }
}
static const char *keyword_names[] = {
    "neu",  // Tương ứng TOKEN_NEU (0)
    "thi",  // Tương ứng TOKEN_THI (1)
    "het",  // Tương ứng TOKEN_HET (2)
    "khi",  // Tương ứng TOKEN_KHI (3)
    "trang" // Tương ứng TOKEN_TRANG (4)
};
static const int keyword_count =
    sizeof(keyword_names) / sizeof(keyword_names[0]);

static TU_KHOA identifier_type() {
  int length = (int)(scanner.current - scanner.start);

  for (int i = 0; i < keyword_count; i++) {
    // Kiểm tra độ dài + nội dung
    if (strlen(keyword_names[i]) == length &&
        memcmp(scanner.start, keyword_names[i], length) == 0) {
      return (TU_KHOA)i;
    }
  }
  return TOKEN_VAR;
}

TOKEN make_token(TU_KHOA type) {
  TOKEN token;
  token.type = type;
  token.start = scanner.start;
  token.length = (int)(scanner.current - scanner.start);
  token.line = scanner.current_line;
  return token;
}

// Hàm này dùng khi gặp ký tự lạ không hiểu
TOKEN error_token(const char *message) {
  TOKEN token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = (int)strlen(message);
  token.line = scanner.current_line;
  return token;
}

TOKEN scan_token() {
  skip_whitespace();
  scanner.start = scanner.current;
  if (is_at_end())
    return make_token(TOKEN_EOF);
  char c = *scanner.current++;
  if (isalpha(c)) {
    // Ăn tiếp các ký tự chữ hoặc số hoặc gạch dưới phía sau
    while (isalnum(*scanner.current) || *scanner.current == '_') {
      scanner.current++;
    }
    // Gọi hàm tra bảng bạn vừa viết để xem là Keyword hay Var
    TU_KHOA type = identifier_type();
    return make_token(type);
  }
  if (isdigit(c)) {
    while (isdigit(*scanner.current)) {
      scanner.current++;
    }
    return make_token(TOKEN_NUMBER);
  }
  switch (c) {
  case ';':
    return make_token(TOKEN_SEMICOLON);
  case '+':
    return make_token(TOKEN_PLUS);
  case '-':
    return make_token(TOKEN_MINUS);
  case '*':
    return make_token(TOKEN_STAR);
  case '/':
    return make_token(TOKEN_SLASH);
  case '<':
    return make_token(TOKEN_LESS);
  case '>':
    return make_token(TOKEN_GREATER);
  case '=':
    if (*scanner.current == '=') {
      scanner.current++;
      return make_token(TOKEN_EQUAL_EQUAL);
    } else {
      return make_token(TOKEN_EQUAL);
    }
  case '\n':
    scanner.current_line++;
    return make_token(TOKEN_NEWLINE);
  }

  return error_token("Ky tu khong hop le.");
}

typedef struct {
  TOKEN current_token;
  TOKEN previous_token; // token vừa đi qua (để nhớ tên biến)
  int error_flag;
} PARSER;
PARSER parser;
void init_parser() {
  parser.error_flag = 0;
  parser.current_token = scan_token();
  parser.previous_token = parser.current_token;
}
void advance_parser() {
  parser.previous_token = parser.current_token;
  parser.current_token = scan_token();
}
void error_parser(TOKEN *token, const char *message) {
  fprintf(stderr, "[Dong %d] Loi", token->line);

  if (token->type == TOKEN_EOF) {
    fprintf(stderr, " tai cuoi file");
  } else if (token->type == TOKEN_ERROR) {
  } else {
    fprintf(stderr, " tai '%.*s'", token->length, token->start);
  }
  fprintf(stderr, ": %s\n", message);
  exit(1);
}

void consume(TU_KHOA type, const char *message) {
  if (parser.current_token.type == type) {
    advance_parser();
    return;
  }
  error_parser(&parser.current_token, message);
}
void consume_terminator() {
  if (parser.current_token.type == TOKEN_SEMICOLON) {
    advance_parser();
  } else if (parser.current_token.type == TOKEN_NEWLINE) {
    advance_parser();
  } else if (parser.current_token.type == TOKEN_EOF) {
    return;
  } else {
    error_parser(&parser.current_token,
                 "Thieu dau ';' hoac xuong dong de ket thuc lenh.");
  }
}

void statement();
void emit(uint8_t opcode, int operand) {
    add_instruction(scanner.prog, opcode, operand);
}
int get_var_index() { return 0; }
#define MAX_VARS 1024
typedef struct {
    char name[32]; // Tên biến tối đa 31 ký tự
    int index;     // Index tương ứng trong VM
} Symbol;
Symbol symbol_table[MAX_VARS];
int symbol_count = 0;
void expression() {}
// ham chuyen xu ly cau lenh
void declare_variable() {
  int global_index = get_var_index();
  consume(TOKEN_EQUAL, "khai bao bien can co dau '='");
  expression();
  consume_terminator();
  emit(CAT, global_index);
}
void if_statement() {
  advance_parser(); // Ăn chữ 'neu'
  expression();     // Tính điều kiện

  consume(TOKEN_THI, "Thieu 'thi' sau dieu kien.");

  // --- BƯỚC 1: SINH LỆNH NHẢY NHÁP ---
  // Ghi lệnh Nhảy Nếu Sai (NNS) với tham số tạm là -1 (hoặc 0xFFFF)
  emit(NNS, -1);
  int jump_idx = scanner.prog->code_size - 1;

  // --- BƯỚC 3: DỊCH KHỐI LỆNH ---
  while (parser.current_token.type != TOKEN_HET &&
         parser.current_token.type != TOKEN_EOF) {
    statement();
  }

  consume(TOKEN_HET, "Thieu 'het' de dong khoi if.");

  // --- BƯỚC 4: VÁ LỖ (Backpatching) ---
  // Đích đến là dòng lệnh tiếp theo sắp được viết (chính là code_size hiện tại)
  int jump_target = scanner.prog->code_size;

  // Quay lại vị trí cũ, sửa tham số -1 thành jump_target
  scanner.prog->code[jump_idx].operand = jump_target;
}

void trang_statement() {
  advance_parser(); // Ăn chữ 'trang'

  emit(VAO_TRANG, 0); // Snapshot

  while (parser.current_token.type != TOKEN_HET &&
         parser.current_token.type != TOKEN_EOF) {
    statement();
  }

  consume(TOKEN_HET, "Thieu 'het' de dong trang.");

  emit(RA_TRANG, 0); // Rollback
}
void khi_statement() {
    // --- BƯỚC 1: ĐÁNH DẤU ĐẦU VÒNG LẶP ---
    // Để lát nữa lệnh NHAY có thể quay về đây kiểm tra điều kiện lại
    int loop_start = scanner.prog->code_size;

    advance_parser(); // Ăn chữ 'khi'
    expression();     // Tính điều kiện

    consume(TOKEN_THI, "Thieu 'thi' sau dieu kien.");

    // --- BƯỚC 2: (Nhảy Nháp) ---
    // Nếu điều kiện sai, nhảy vọt ra ngoài
    emit(NNS, -1); 
    int exit_jump_idx = scanner.prog->code_size - 1; // Lưu vị trí cái lỗ

    // --- BƯỚC 3: DỊCH THÂN VÒNG LẶP ---
    while (parser.current_token.type != TOKEN_HET && 
           parser.current_token.type != TOKEN_EOF) {
        statement();
    }

    consume(TOKEN_HET, "Thieu 'het' de dong vong lap.");

    // --- BƯỚC 4: QUAY XE (Loop Back) ---
    // Nhảy vô điều kiện về đầu vòng lặp
    emit(NHAY, loop_start);

    // --- BƯỚC 5: VÁ LỐI THOÁT ---
    // Bây giờ mới biết điểm kết thúc vòng lặp ở đâu -> Sửa lệnh NNS ban nãy
    int loop_end = scanner.prog->code_size;
    scanner.prog->code[exit_jump_idx].operand = loop_end;
}
void statement() {

  if (parser.current_token.type == TOKEN_NEWLINE) {
    advance_parser();
    return;
  }
  if (parser.current_token.type == TOKEN_VAR) {
    declare_variable();
  } else if (parser.current_token.type == TOKEN_NEU) {
    if_statement();
    advance_parser();
  } else if (parser.current_token.type == TOKEN_TRANG) {
    trang_statement();
  } else if (parser.current_token.type == TOKEN_KHI) {
    khi_statement();
  } else {
    error_parser(&parser.current_token, "khong hieu lenh nay.");
  }
}
void compile(CHUONG_TRINH *program, const char *source) {
  scanner.prog = program;
  init_scanner(source);
  init_parser();
  while (parser.current_token.type != TOKEN_EOF) {
    statement();
  }
  emit(DUNG, 0);
}
