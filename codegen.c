#include "2cc.h"
#include <stdio.h>
#include <stdlib.h>

Node *code[100];
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

void program() {
  int i = 0;

  while (!at_eof()) {
    code[i++] = stmt();
    code[i] = NULL;
  }
}

Node *stmt() {
  Node *node = expr();
  expect(";");
  return node;
}

Node *expr() {
  Node *node = assign();
  return node;
}

Node *assign() {
  Node *node = eql();
  if (consume("="))
    node = new_node(ND_ASSIGN, node, assign());
  return node;
}

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

  Token *tok = consume_ident();
  if (tok) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_LVAR;
    // 変数1文字で考えているので、強制的にoffsetを決められる
    node->offset = (tok->str[0] - 'a' + 1) * 8;
    return node;
  }

  return new_node_num(expect_number());
}

void gen_lval(Node *node) {
  if (node->kind != ND_LVAR)
    perror("trying to assign a non-lhs type char");

  printf("  mov rax, rbp\n");
  printf("  sub rax, %d\n", node->offset);
  printf("  push rax\n");
}

void gen(Node *node) {
  switch (node->kind) {
  case (ND_NUM):
    printf("  push %d\n", node->val);
    return;
  case (ND_LVAR):
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax [rax]\n");
    printf("  push rax\n");
    return;

  case (ND_ASSIGN):
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
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
