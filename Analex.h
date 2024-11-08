#ifndef ANALEX
#define ANALEX

#define TAM_MAX_LEXEMA 31
#define NUM_PALAVRAS_RESERVADAS 28

enum TOKEN_CAT {
  ID = 1,
  SN,
  CT_I,
  CT_R,
  CT_C,
  LT,
  PR,
  FIM_EXPR,
  FIM_ARQ,
  ERRO
};

enum SINAISP {
  ATRIBUICAO = 1,
  ADICAO,
  SUBTRACAO,
  MULTIPLICACAO,
  DIVISAO,
  ABRE_PAR,
  FECHA_PAR,
  ABRE_COL,
  FECHA_COL,
  MAIOR,
  MENOR,
  MAIOR_IGUAL,
  MENOR_IGUAL,
  IGUALDADE,
  DIFERENTE,
  ENDERECO,
  VIRGULA,
  COND_ADICAO,
  COND_ALTERNATIVA,
  COND_NEGACAO
};

enum PalavrasReservadas {
  CONST = 1,
  Pr,
  INIT,
  END,
  CHAR,
  INT,
  REAL,
  BOOL,
  DO,
  WHILE,
  ENDW,
  VAR,
  FROM,
  TO,
  DT,
  BY,
  IF,
  ENDV,
  ELSE,
  ELIF,
  ENDI,
  GETOUT,
  GETINT,
  GETREAL,
  GETCHAR,
  PUTINT,
  PUTREAL,
  PUTCHAR
};

char palavras_reservadas[NUM_PALAVRAS_RESERVADAS][20] = {
    "const",  "pr",     "init",    "end",     "char",   "int",     "real",
    "bool",   "do",     "while",   "endw",    "var",    "from",    "to",
    "dt",     "by",     "if",      "endv",    "else",   "elif",    "endi",
    "getout", "getint", "getreal", "getchar", "putint", "putreal", "putchar"};

typedef struct {
  enum TOKEN_CAT cat;
  union {
    int codigo;
    char lexema[TAM_MAX_LEXEMA];
    int valInt;
    float valFloat;
  };
} TOKEN;

#endif

int contLinha = 1;
