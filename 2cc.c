#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum {
  TK_RESERVED, //記号
  TK_NUM,      //数値
  TK_EOF,      //終わり
} TokenKind;

typedef struct Token Token;

// Token型
struct Token {
  TokenKind kind; //型
  Token *next;
  int val;   // kind == TK_NUMのときの数値
  char *str; // トークン文字列
  int len;   // トークンの長さ
};

// ASTのノードの種類
typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM, // せいすう
  ND_NEQ, // !=
  ND_EQ,  // ==
  ND_LEQ, // <=
  ND_LES, // <
  ND_GEQ, // >=
  ND_GRE  // >
} NodeKind;

typedef struct Node Node;

// Node型
struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
};

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

bool consume(char *op) {
  // 記号でないか、期待している記号でないなら偽
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  // 次のトークンに進める
  token = token->next;
  return true;
}

void expect(char *op) {
  // 記号でないか、期待している記号でないなら偽
  if (token->kind != TK_RESERVED || strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    error_at(token->str, "expected operator '%c' but instead got '%c'", op,
             token->str[0]);
  token = token->next;
}

int expect_number() {
  if (token->kind != TK_NUM)
    error_at(token->str, "consumed non-numeric value");
  int val = token->val;
  token = token->next;
  return val;
}

bool at_eof() { return (token->kind == TK_EOF); }

//可視化用

void show_node(Node *node) {
  switch (node->kind) {
  case ND_ADD:
    printf("current nodeval: %s\n", "ADD");
    break;
  case ND_SUB:
    printf("current nodeval: %s\n", "SUB");
    break;
  case ND_DIV:
    printf("current nodeval: %s\n", "DIV");
    break;
  case ND_MUL:
    printf("current nodeval: %s\n", "MUL");
    break;
  case ND_NUM:
    printf("current nodeval: %s, value: %d\n", "NUM", node->val);

    break;
  }
  show_node(node->lhs);
  show_node(node->rhs);
}

Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

Node *eql();
Node *rel();
Node *add();
Node *mul();
Node *primary();
Node *unary();

Node *eql() {
  Node *node = rel();
  while (1) {
    if (consume("=="))
      node = new_node(ND_EQ, node, rel());
    else if (consume("!="))
      node = new_node(ND_NEQ, node, rel());
    else
      return node;
  }
}

Node *rel() {
  Node *node = add();
  while (1) {
    if (consume("<="))
      node = new_node(ND_LEQ, node, mul());
    else if (consume(">="))
      node = new_node(ND_GEQ, node, mul());
    else if (consume("<"))
      node = new_node(ND_LES, node, mul());
    else if (consume(">"))
      node = new_node(ND_GRE, node, mul());
    else
      return node;
  }
}

Node *add() {
  Node *node = mul();
  while (1) {
    if (consume("+"))
      node = new_node(ND_ADD, node, mul());
    else if (consume("-"))
      node = new_node(ND_SUB, node, mul());
    else
      return node;
  }
}

Node *mul() {
  Node *node = unary();
  while (1) {
    if (consume("*"))
      node = new_node(ND_MUL, node, unary());
    else if (consume("/"))
      node = new_node(ND_DIV, node, unary());
    else
      return node;
  }
}

Node *unary() {
  if (consume("+"))
    return primary();
  if (consume("-"))
    return new_node(ND_SUB, new_node_num(0), primary());
  return primary();
}

Node *primary() {
  if (consume("(")) {
    Node *node = eql();
    expect(")");
    return node;
  }

  return new_node_num(expect_number());
}

Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

Token *tokenize(char *p) {
  Token head;
  head.next = NULL;
  Token *cur = &head;

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    if (!memcmp(p, "==", 2) || !memcmp(p, "!=", 2) || !memcmp(p, "<=", 2) ||
        !memcmp(p, ">=", 2)) {
      cur = new_token(TK_RESERVED, cur, p++);
      cur->len = 2;
      p++;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '<' || *p == '>') {
      cur = new_token(TK_RESERVED, cur, p++);
      cur->len = 1;
      continue;
    }

    if (isdigit(*p)) {
      cur = new_token(TK_NUM, cur, p);
      cur->val = strtol(p, &p, 10);
      continue;
    }
    error_at(p, "could not tokenize");
  }
  new_token(TK_EOF, cur, p);
  return head.next;
}

void show_token(Token *token) {
  switch (token->kind) {
  case (TK_NUM):
    printf("val: %d, kind: NUM\n", token->val);
    break;
  case (TK_RESERVED):
    printf("kind: RESERVED\n", token->val);
    break;
  case (TK_EOF):
    printf("kind: EOF\n", token->val);
    break;
  }
  if (token->next != NULL)
    show_token(token->next);
}

void gen(Node *node) {
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_EQ:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_NEQ:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LEQ:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_LES:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_GEQ:
    printf("  cmp rax, rdi\n");
    printf("  setge al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_GRE:
    printf("  cmp rax, rdi\n");
    printf("  setg al\n");
    printf("  movzb rax, al\n");
    break;
  }

  printf("  push rax\n");
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
