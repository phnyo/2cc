#include "2cc.h"

// 現在のトークンへのポインタ
Token *token;

// 入力されたプログラムの文字列
char *user_input;

void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  // char型の大きさが1bitなのでlocとuser_inputの差はそのままintにできる
  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  // pos個の空白が入る
  fprintf(stderr, "%*s", pos, " ");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "wrong number of arguments\n");
    return 1;
  }

  user_input = argv[1];
  token = tokenize(argv[1]);
  // show_token(token);

  Node *node = eql();
  // show_node(node);

  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");

  return 0;
}
