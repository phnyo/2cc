#include "2cc.h"

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
