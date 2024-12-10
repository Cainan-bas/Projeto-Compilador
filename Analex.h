#ifndef ANALEX
#define ANALEX

#include <stdio.h>
#include <stdbool.h>

#define TAM_MAX_LEXEMA 31
#define NUM_PALAVRAS_RESERVADAS 31

enum TOKEN_CAT {
  ID = 1,
  SN,
  CT_I,
  CT_R,
  CT_C,
  CT_STR,
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
  ABRE_CHAVES,
  FECHA_CHAVES,
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
  PROT,
  DEF,
  INIT,
  ENDP, // perguntar pra alysson
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
  GETSTR,
  PUTINT,
  PUTREAL,
  PUTCHAR,
  PUTSTR
};

char palavras_reservadas[NUM_PALAVRAS_RESERVADAS][20];

void error(char msg[]);

typedef struct {
  enum TOKEN_CAT cat;
  bool processado;
  union {
    int codigo;
    char lexema[TAM_MAX_LEXEMA];
    int valInt;
    float valFloat;
  };
} TOKEN;

/* Variaveis globais */
extern TOKEN t;
extern FILE *fd;
extern int contLinha;  // Contador de linhas do código fonte 

/* Assinaturas de funcoes */
TOKEN Analex(FILE *);
void Print_Analex(TOKEN);

#endif