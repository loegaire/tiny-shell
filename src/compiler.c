#include "../include/interpreter.h"
#include "../include/shell.h"

typedef enum {
  TOKEN_NEU,
  TOKEN_THI,
  TOKEN_HET,
  TOKEN_KHI,
  TOKEN_TRANG,
  TOKEN_IN,
  TOKEN_EQUAL,
  TOKEN_SEMICOLON, // = ;
  TOKEN_PLUS,
  TOKEN_MINUS,
  TOKEN_STAR,
  TOKEN_SLASH, // + - * /
  TOKEN_LESS,
  TOKEN_GREATER,
  TOKEN_EQUAL_EQUAL, // < > ==
  TOKEN_LPAREN,
  TOKEN_RPAREN,
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
  // IMPORTANT: do NOT reset scanner.prog here; compile() owns that.
}

int is_at_end() { return *scanner.current == '\0'; }
void advance() { scanner.current++; }

void skip_whitespace() {
  for (;;) {
    char c = *scanner.current;

    // Support both '#' and '//' comments.
    if (c == '/' && scanner.current[1] == '/') {
      while (*scanner.current != '\n' && !is_at_end()) {
        advance();
      }
      continue;
    }

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
    "neu",  // TOKEN_NEU (0)
    "thi",  // TOKEN_THI (1)
    "het",  // TOKEN_HET (2)
    "khi",  // TOKEN_KHI (3)
  "trang", // TOKEN_TRANG (4)
  "in"     // TOKEN_IN (5)
};
static const int keyword_count =
    sizeof(keyword_names) / sizeof(keyword_names[0]);

static TU_KHOA identifier_type() {
  int length = (int)(scanner.current - scanner.start);

  for (int i = 0; i < keyword_count; i++) {
    if ((int)strlen(keyword_names[i]) == length &&
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

  if (isalpha((unsigned char)c)) {
    while (isalnum((unsigned char)*scanner.current) || *scanner.current == '_') {
      scanner.current++;
    }
    TU_KHOA type = identifier_type();
    return make_token(type);
  }

  if (isdigit((unsigned char)c)) {
    while (isdigit((unsigned char)*scanner.current)) {
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
  case '(':
    return make_token(TOKEN_LPAREN);
  case ')':
    return make_token(TOKEN_RPAREN);
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

// Add 1-token lookahead to disambiguate:
// - assignment:   x = ...
// - expression:   x + 1
// - command:      date
typedef struct {
  TOKEN current_token;
  TOKEN next_token;
  TOKEN previous_token;
  int error_flag;
} PARSER;

PARSER parser;

void init_parser() {
  parser.error_flag = 0;
  parser.current_token = scan_token();
  parser.next_token = scan_token();
  parser.previous_token = parser.current_token;
}

void advance_parser() {
  parser.previous_token = parser.current_token;
  parser.current_token = parser.next_token;
  parser.next_token = scan_token();
}

void error_parser(TOKEN *token, const char *message) {
  fprintf(stderr, "[Dong %d] Loi", token->line);

  if (token->type == TOKEN_EOF) {
    fprintf(stderr, " tai cuoi file");
  } else if (token->type == TOKEN_ERROR) {
    // already has message in token->start
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
  } else if (parser.current_token.type == TOKEN_HET) {
    // Allow omitting ';' or newline before closing a block.
    return;
  } else if (parser.current_token.type == TOKEN_EOF) {
    return;
  } else {
    error_parser(&parser.current_token,
                 "Thieu dau ';' hoac xuong dong de ket thuc lenh.");
  }
}

void statement();

static void emit(uint8_t opcode, int operand) {
  if (scanner.prog == NULL) {
    fprintf(stderr, "Internal error: scanner.prog is NULL\n");
    exit(1);
  }
  add_instruction(scanner.prog, opcode, operand);
}

static int emit_constant(int value) {
  add_constant(scanner.prog, value);
  int idx = scanner.prog->constants_size - 1;
  emit(NAP, idx);
  return idx;
}

#define MAX_VARS 1024
typedef struct {
  char name[32]; // Tên biến tối đa 31 ký tự
  int index;     // Index tương ứng trong VM
} Symbol;

static Symbol symbol_table[MAX_VARS];
static int symbol_count = 0;

// Reserve the last VM var slot as a dedicated return register for `trang`.
// This avoids stack underflow issues when the "last expression" is conditional.
enum { TRANG_RET_SLOT = 1023 };

static int symbol_lookup_or_add(const TOKEN *nameTok) {
  int n = nameTok->length;
  if (n <= 0) {
    error_parser((TOKEN *)nameTok, "Ten bien khong hop le.");
  }
  if (n >= (int)sizeof(symbol_table[0].name)) {
    error_parser((TOKEN *)nameTok, "Ten bien qua dai (toi da 31 ky tu).");
  }

  for (int i = 0; i < symbol_count; i++) {
    if ((int)strlen(symbol_table[i].name) == n &&
        memcmp(symbol_table[i].name, nameTok->start, n) == 0) {
      return symbol_table[i].index;
    }
  }

  // Keep one slot reserved for TRANG_RET_SLOT.
  if (symbol_count >= TRANG_RET_SLOT) {
    error_parser((TOKEN *)nameTok, "Qua nhieu bien (vuot 1023 vi tri su dung duoc).");
  }

  memcpy(symbol_table[symbol_count].name, nameTok->start, (size_t)n);
  symbol_table[symbol_count].name[n] = '\0';
  symbol_table[symbol_count].index = symbol_count;

  symbol_count++;
  return symbol_count - 1;
}

// -------------------- Expression compiler --------------------
// Grammar (rough):
// expr        -> equality
// equality    -> comparison ( "==" comparison )*
// comparison  -> term ( ( "<" | ">" ) term )*
// term        -> factor ( ( "+" | "-" ) factor )*
// factor      -> unary ( ( "*" | "/" ) unary )*
// unary       -> "-" unary | primary
// primary     -> NUMBER | VAR | "(" expr ")"

static void expression();

static int in_trang = 0;

static void primary() {
  if (parser.current_token.type == TOKEN_NUMBER) {
    int value = atoi(parser.current_token.start);
    advance_parser();
    emit_constant(value);
    return;
  }

  if (parser.current_token.type == TOKEN_VAR) {
    TOKEN nameTok = parser.current_token;
    advance_parser();
    int slot = symbol_lookup_or_add(&nameTok);
    emit(LAY, slot);
    return;
  }

  if (parser.current_token.type == TOKEN_LPAREN) {
    advance_parser();
    expression();
    consume(TOKEN_RPAREN, "Thieu ')' de dong bieu thuc.");
    return;
  }

  if (parser.current_token.type == TOKEN_TRANG) {
    // `trang ... het` as an expression:
    // - enter sandbox (fresh stack/vars)
    // - execute statements, storing the "result" into TRANG_RET_SLOT
    // - push TRANG_RET_SLOT and exit sandbox
    advance_parser(); // eat 'trang'
    emit(VAO_TRANG, 0);

    // Initialize return value to 0.
    emit_constant(0);
    emit(CAT, TRANG_RET_SLOT);

    in_trang++;
    while (parser.current_token.type != TOKEN_HET &&
           parser.current_token.type != TOKEN_EOF) {
      statement();
    }
    in_trang--;

    consume(TOKEN_HET, "Thieu 'het' de dong trang.");

    emit(LAY, TRANG_RET_SLOT);
    emit(RA_TRANG, 0);
    return;
  }

  error_parser(&parser.current_token, "Thieu toan hang (so, bien, hoac '()').");
}

static void unary() {
  if (parser.current_token.type == TOKEN_MINUS) {
    advance_parser();
    // Compile: 0 - expr
    emit_constant(0);
    unary();
    emit(TRU, 0);
    return;
  }
  primary();
}

static void factor() {
  unary();
  while (parser.current_token.type == TOKEN_STAR ||
         parser.current_token.type == TOKEN_SLASH) {
    TU_KHOA op = parser.current_token.type;
    advance_parser();
    unary();
    if (op == TOKEN_STAR) emit(NHAN, 0);
    else emit(CHIA, 0);
  }
}

static void term() {
  factor();
  while (parser.current_token.type == TOKEN_PLUS ||
         parser.current_token.type == TOKEN_MINUS) {
    TU_KHOA op = parser.current_token.type;
    advance_parser();
    factor();
    if (op == TOKEN_PLUS) emit(CONG, 0);
    else emit(TRU, 0);
  }
}

static void comparison() {
  term();
  while (parser.current_token.type == TOKEN_LESS ||
         parser.current_token.type == TOKEN_GREATER) {
    TU_KHOA op = parser.current_token.type;
    advance_parser();
    term();
    if (op == TOKEN_LESS) emit(NHO, 0);
    else emit(LON, 0);
  }
}

static void equality() {
  comparison();
  while (parser.current_token.type == TOKEN_EQUAL_EQUAL) {
    advance_parser();
    comparison();
    emit(BANG, 0);
  }
}

static void expression() {
  equality();
}

// -------------------- Statements --------------------

static void declare_variable() {
  // Current token is TOKEN_VAR at statement start
  TOKEN nameTok = parser.current_token;
  int slot = symbol_lookup_or_add(&nameTok);
  advance_parser(); // eat variable name

  consume(TOKEN_EQUAL, "Khai bao/gan bien can co dau '='.");
  expression();
  consume_terminator();
  emit(CAT, slot);
}

static void expression_statement() {
  expression();
  consume_terminator();

  if (in_trang > 0) {
    // In `trang`, treat expression statements as "set return value".
    emit(CAT, TRANG_RET_SLOT);
  } else {
    // Outside, discard the computed value to avoid leaking stack.
    emit(BO, 0);
  }
}

static void if_statement() {
  advance_parser(); // eat 'neu'
  expression();

  consume(TOKEN_THI, "Thieu 'thi' sau dieu kien.");

  emit(NNS, -1);
  int jump_idx = scanner.prog->code_size - 1;

  while (parser.current_token.type != TOKEN_HET &&
         parser.current_token.type != TOKEN_EOF) {
    statement();
  }

  consume(TOKEN_HET, "Thieu 'het' de dong khoi if.");

  int jump_target = scanner.prog->code_size;
  scanner.prog->code[jump_idx].operand = jump_target;
}

static void in_statement() {
  advance_parser(); // eat 'in'
  expression();
  consume_terminator();
  emit(IN, 0);
}

static void trang_statement() {
  // Allow standalone `trang ... het` as a statement.
  // Compile it as an expression (produces a value), then discard it.
  expression();
  consume_terminator();
  emit(BO, 0);
}

static void khi_statement() {
  int loop_start = scanner.prog->code_size;

  advance_parser(); // eat 'khi'
  expression();
  consume(TOKEN_THI, "Thieu 'thi' sau dieu kien.");

  emit(NNS, -1);
  int exit_jump_idx = scanner.prog->code_size - 1;

  while (parser.current_token.type != TOKEN_HET &&
         parser.current_token.type != TOKEN_EOF) {
    statement();
  }

  consume(TOKEN_HET, "Thieu 'het' de dong vong lap.");

  emit(NHAY, loop_start);

  int loop_end = scanner.prog->code_size;
  scanner.prog->code[exit_jump_idx].operand = loop_end;
}

static int is_expr_continuation(TU_KHOA t) {
  switch (t) {
    case TOKEN_PLUS:
    case TOKEN_MINUS:
    case TOKEN_STAR:
    case TOKEN_SLASH:
    case TOKEN_LESS:
    case TOKEN_GREATER:
    case TOKEN_EQUAL_EQUAL:
    case TOKEN_RPAREN:
    case TOKEN_LPAREN:
      return 1;
    default:
      return 0;
  }
}

static void command_statement() {
  // current token is command name (identifier)
  TOKEN cmdTok = parser.current_token;
  advance_parser(); // eat command name

  consume_terminator();

  int cmdIndex = add_command(scanner.prog, cmdTok.start, cmdTok.length);
  // push cmdIndex, then IN 1 executes it
  emit_constant(cmdIndex);
  emit(IN, 1);
}

void statement() {
  if (parser.current_token.type == TOKEN_NEWLINE) {
    advance_parser();
    return;
  }

  if (parser.current_token.type == TOKEN_IN) {
    in_statement();
    return;
  }

  if (parser.current_token.type == TOKEN_NEU) {
    if_statement();
    return;
  }

  if (parser.current_token.type == TOKEN_TRANG) {
    trang_statement();
    return;
  }

  if (parser.current_token.type == TOKEN_KHI) {
    khi_statement();
    return;
  }

  if (parser.current_token.type == TOKEN_VAR) {
    // x = ... (assignment)
    if (parser.next_token.type == TOKEN_EQUAL) {
      declare_variable();
      return;
    }

    // In trang, `x + 1` etc must remain expression statements (return value).
    if (in_trang > 0 && is_expr_continuation(parser.next_token.type)) {
      expression_statement();
      return;
    }

    // Outside trang, allow expression statements too (discard), when clearly an expr.
    if (is_expr_continuation(parser.next_token.type)) {
      expression_statement();
      return;
    }

    // Otherwise: treat as shell command (single-word, like `date`)
    command_statement();
    return;
  }

  // Allow expression statements that don't start with an identifier.
  // Examples: `123`, `-1`, `(a + b)`.
  if (parser.current_token.type == TOKEN_NUMBER ||
      parser.current_token.type == TOKEN_LPAREN ||
      parser.current_token.type == TOKEN_MINUS) {
    expression_statement();
    return;
  }

  error_parser(&parser.current_token, "Khong hieu lenh nay.");
}

void compile(CHUONG_TRINH *program, const char *source) {
  init_scanner(source);
  scanner.prog = program;

  // reset symbol table per compile
  symbol_count = 0;

  init_parser();
  while (parser.current_token.type != TOKEN_EOF) {
    statement();
  }
  emit(DUNG, 0);
}
