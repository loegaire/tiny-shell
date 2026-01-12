#include "../include/shell.h"
#include "../include/interpreter.h"

static char *read_entire_file(const char *path) {
  FILE *f = fopen(path, "rb");
  if (!f) return NULL;

  if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return NULL; }
  long n = ftell(f);
  if (n < 0) { fclose(f); return NULL; }
  rewind(f);

  char *buf = (char *)malloc((size_t)n + 1);
  if (!buf) { fclose(f); return NULL; }

  size_t got = fread(buf, 1, (size_t)n, f);
  fclose(f);

  if (got != (size_t)n) { free(buf); return NULL; }
  buf[n] = '\0';
  return buf;
}

int builtin_trang(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "cach dung: trang <file.trang>\n");
    return 1;
  }

  char *source = read_entire_file(args[1]);
  if (!source) {
    perror("trang: khong mo/doc duoc file");
    return 1;
  }

  CHUONG_TRINH prog;
  init_CHUONG_TRINH(&prog);

  compile(&prog, source);

  MAY_AO vm;
  init_MAY_AO(&vm, &prog);
  run(&vm);

  free(source);
  free(prog.code);
  free(prog.constants);

  if (prog.cmds) {
    for (int i = 0; i < prog.cmds_size; i++) {
      free(prog.cmds[i]);
    }
    free(prog.cmds);
  }
  return 1; // builtin handled
}