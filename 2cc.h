#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// トークンの種類
typedef enum {
  TK_RESERVED, //記号
  TK_IDENT,    // Identifier = 変数
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
  ND_ADD,    // +
  ND_SUB,    // -
  ND_MUL,    // *
  ND_DIV,    // /
  ND_NUM,    // せいすう
  ND_NEQ,    // !=
  ND_EQ,     // ==
  ND_LEQ,    // <=
  ND_LES,    // <
  ND_GEQ,    // >=
  ND_GRE,    // >
  ND_ASSIGN, // =
  ND_LVAR,   // local variable
} NodeKind;

typedef struct Node Node;

// Node型
struct Node {
  NodeKind kind;
  Node *lhs;
  Node *rhs;
  int val;
  int offset;
};

typedef struct LVar LVar;

struct LVar {
  LVar *next; // 次の変数
  char *name; // なまえ
  int len;    // 名前の長さ
  int offset; // rbpからのoffset
};

// eBNFをそのまま関数にしたやつ
void program();
Node *stmt();
Node *expr();
Node *assign();
Node *eql();
Node *rel();
Node *add();
Node *mul();
Node *primary();
Node *unary();

// トークン処理
void gen(Node *node);
bool consume(char *op);
Token *consume_ident();
void expect(char *op);
int expect_number();
Token *tokenize(char *p);
bool at_eof();
LVar *find_lvar(Token *token);

// エラー出力
void error_at(char *loc, char *fmt, ...);

// 現在のトークンへのポインタ
extern Token *token;

// 入力されたプログラムの文字列
extern char *user_input;

// stmtの保存配列
extern Node *code[100];

// 静的変数
extern LVar *locals;
