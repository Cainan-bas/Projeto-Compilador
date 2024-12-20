#include "Analex.h"
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_LEXEMA 50
#define TAM_NUM 20

char palavras_reservadas[NUM_PALAVRAS_RESERVADAS][20] = {
    "const", "prot","def",     "init",    "endp",     "char",   "int",     "real",
    "bool",   "do",     "while",   "endw",    "var",    "from",    "to",
    "dt",     "by",     "if",      "endv",    "else",   "elif",    "endi",
    "getout", "getint", "getreal", "getchar", "getstr","putint", "putreal", "putchar", "putstr"};

int comparaPalavraReservada(char *palavra) {
  for (int i = 0; i < NUM_PALAVRAS_RESERVADAS; i++) {
    if (strcmp(palavra, palavras_reservadas[i]) == 0) {
      return i+1;
    }
  }
  return 0;
}

void error(char msg[]) {
  printf("LINHA %d -> %s\n", contLinha+1 ,msg);
  exit(1);
}

TOKEN Analex(FILE *fd) {
  int estado;
  char lexema[TAM_LEXEMA] = "";
  int tamL = 0;
  char digitos[TAM_NUM] = "";
  int tamD = 0;
  TOKEN t;

  estado = 0;
  while (true) {
    char c = fgetc(fd);
    switch (estado) {
    case 0:
      if (c == ' ' || c == '\t') {
        estado = 0;
      } else if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') {
        estado = 1;
        lexema[tamL] = c;
        lexema[++tamL] = '\0';
      } else if (c == '_') {
        estado = 2;
        lexema[tamL] = c;
        lexema[++tamL] = '\0';
      } else if (c >= '0' && c <= '9') {
        estado = 4;
        digitos[tamD] = c;
        digitos[++tamD] = '\0';
      } else if (c == '\'') { // codigo ascii 39
        estado = 8;
        lexema[tamL] = c;
        lexema[++tamL] = '\0';
      } else if (c == '"') { // codigo das ASPAS "
        estado = 11;
        lexema[tamL] = c;
        lexema[++tamL] = '\0';
      } else if (c == '+') {
        estado = 14;
        t.cat = SN;
        t.codigo = ADICAO;
        return t;
      } else if (c == '-') {
        estado = 15;
        t.cat = SN;
        t.codigo = SUBTRACAO;
        return t;
      } else if (c == '*') {
        estado = 16;
        t.cat = SN;
        t.codigo = MULTIPLICACAO;
        return t;
      } else if (c == '/') {
        estado = 17;
      } else if (c == '=') {
        estado = 39;
      } else if (c == '<') {
        estado = 20;
      } else if (c == '>') {
        estado = 33;
      } else if (c == '!') {
        estado = 36;
      } else if (c == '&') {
        estado = 24;
      } else if (c == '|') {
        estado = 27;
      } else if (c == ',') {
        estado = 23;
        t.cat = SN;
        t.codigo = VIRGULA;
        return t;
      } else if (c == '(') {
        estado = 29;
        t.cat = SN;
        t.codigo = ABRE_PAR;
        return t;
      } else if (c == ')') {
        estado = 30;
        t.cat = SN;
        t.codigo = FECHA_PAR;
        return t;
      } else if (c == '{') {
        estado = 46;
        t.cat = SN;
        t.codigo = ABRE_CHAVES;
        return t;
      } else if (c == '}') {
        estado = 47;
        t.cat = SN;
        t.codigo = FECHA_CHAVES;
        return t;
      }
      else if (c == '[') {
        estado = 31;
        t.cat = SN;
        t.codigo = ABRE_COL;
        return t;
      } else if (c == ']') {
        estado = 32;
        t.cat = SN;
        t.codigo = FECHA_COL;
        return t;
      } else if (c == '\n') {
        estado = 0;
        t.cat = FIM_EXPR;
        return t;
      } else if (c == EOF) {
        t.cat = FIM_ARQ;
        return t;
      } else {
        t.cat = ERRO;
        return t;
      }
      break;

    case 1:
      if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9') || (c == '_')) {
        estado = 1;
        lexema[tamL] = c;
        lexema[++tamL] = '\0';
      } else {
        estado = 3;
        ungetc(c, fd);
        if (comparaPalavraReservada(lexema)) {
          t.cat = PR;
          t.codigo = comparaPalavraReservada(lexema);
        } else {
          t.cat = ID;
          strcpy(t.lexema, lexema);
        }
        return t;
      }
      break;
    case 2:
      if (c == '_') {
        estado = 2;
        lexema[tamL] = c;
        lexema[++tamL] = '\0';
      } else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        estado = 1;
        lexema[tamL] = c;
        lexema[++tamL] = '\0';
      } else {
        estado = 50;
      }
      break;
    case 4:
      if (c >= '0' && c <= '9') {
        estado = 4;
        digitos[tamD] = c;
        digitos[++tamD] = '\0';
      } else if (c == '.') {
        estado = 6;
        digitos[tamD] = c;
        digitos[++tamD] = '\0';
      } else {
        estado = 5;
        ungetc(c, fd);
        t.cat = CT_I;
        t.valInt = atoi(digitos);
        return t;
      }
      break;
    case 6:
      if (c >= '0' && c <= '9') {
        estado = 6;
        digitos[tamD] = c;
        digitos[++tamD] = '\0';
      } else {
        estado = 7;
        ungetc(c, fd);
        t.cat = CT_R;
        t.valFloat = atof(digitos);
        return t;
      }
      break;
    case 8:
      if (c == '\\') {
        estado = 42;
        lexema[tamL] = c;
        lexema[++tamL] = '\0';
      } else if (isprint(c)) {
        // } else if (isprint(c) && c != '\n') {
        estado = 9;
        lexema[tamL] = c;
        lexema[++tamL] = '\0';
      } else {
        estado = 50;
      }
      break;
    case 9:
      if (c == '\'') {
        estado = 10;
        lexema[tamL] = c;
        lexema[++tamL] = '\0';
        t.cat = CT_C;
        strcpy(t.lexema, lexema);
        return t;
      } else {
        estado = 50;
      }
      break;
    case 11:
      //if (isprint(c) && c != '\n' && c != '"') {
      //if (((c >= 32 && c <= 126) || (c >= 160)) && c != '\n' && c != '"'){
      if(c != '\n' && c != '"'){
        estado = 11;
        lexema[tamL] = c;
        lexema[++tamL] = '\0';
      } else if (c == '"') {
        estado = 12;
        lexema[tamL] = c;
        lexema[++tamL] = '\0';
        t.cat = CT_STR;
        strcpy(t.lexema, lexema);
        return t;
      } else {
        estado = 50;
      }
      break;
    case 13:
      if (c == '\'') {
        estado = 43;
        t.cat = CT_C;
        t.codigo = '\n';
        return t;
      } else {
        estado = 50;
      }
      break;
    case 17:
      if (c == '/') {
        estado = 18;
      } else {
        estado = 19;
        ungetc(c, fd);
        t.cat = SN;
        t.codigo = DIVISAO;
        return t;
      }
      break;
    case 18:
      if (!(c == '\n')) {
        estado = 18;
      } else {
        estado = 0;
      }
      break;
    case 20:
      if (c == '=') {
        estado = 22;
        t.cat = SN;
        t.codigo = MENOR_IGUAL;
        return t;
      } else {
        estado = 21;
        ungetc(c, fd);
        t.cat = SN;
        t.codigo = MENOR;
        return t;
      }
      break;
    case 24:
      if (c == '&') {
        estado = 25;
        t.cat = SN;
        t.codigo = COND_ADICAO;
        return t;
      } else {
        estado = 26;
        ungetc(c, fd);
        t.cat = SN;
        t.codigo = ENDERECO;
        return t;
      }
      break;
    case 27:
      if (c == '|') {
        estado = 28;
        t.cat = SN;
        t.codigo = COND_ALTERNATIVA;
        return t;
      } else {
        estado = 50;
      }
      break;
    case 33:
      if (c == '=') {
        estado = 35;
        t.cat = SN;
        t.codigo = MAIOR_IGUAL;
        return t;
      } else {
        estado = 34;
        ungetc(c, fd);
        t.cat = SN;
        t.codigo = MAIOR;
        return t;
      }
      break;
    case 36:
      if (c == '=') {
        estado = 37;
        t.cat = SN;
        t.codigo = DIFERENTE;
        return t;
      } else {
        estado = 38;
        ungetc(c, fd);
        t.cat = SN;
        t.codigo = COND_NEGACAO;
        return t;
      }
      break;
    case 39:
      if (c == '=') {
        estado = 41;
        t.cat = SN;
        t.codigo = IGUALDADE;
        return t;
      } else {
        estado = 40;
        ungetc(c, fd);
        t.cat = SN;
        t.codigo = ATRIBUICAO;
        return t;
      }
      break;
    case 42:
      if (c == 'n') {
        estado = 13;
      } else if (c == '0') {
        estado = 44;
      } else {
        estado = 50;
      }
      break;
    case 44:
      if (c == '\'') {
        estado = 45;
        t.cat = CT_C;
        t.codigo = '\0';
        return t;
      } else {
        estado = 50;
      }
      break;
    case 50:
      t.cat = ERRO;
      ungetc(c, fd);
      return t;
      break;
    }
  }
}

void Print_Analex(TOKEN tk) {
  // FILE *fd;
  // TOKEN t;
  // if ((fd = fopen("teste.txt", "r")) == NULL)
  //   error("Arquivo de entrada da expressao nao encontrado!");

  // printf("LINHA %d: ", contLinha);

  while (1) {
    switch (t.cat) {
    case ID:
      printf("<ID, %s> ", t.lexema);
      break;
    case PR:
      switch (tk.codigo) {
      case CONST:
          printf("<PR, CONST> ");
          break;
      case DEF:
          printf("<PR, DEF> ");
          break;
      case INIT:
          printf("<PR, INIT> ");
          break;
      case ENDP:
          printf("<PR, ENDP> ");
          break;
      case CHAR:
          printf("<PR, CHAR> ");
          break;
      case INT:
          printf("<PR, INT> ");
          break;
      case REAL:
          printf("<PR, REAL> ");
          break;
      case BOOL:
          printf("<PR, BOOL> ");
          break;
      case DO:
          printf("<PR, DO> ");
          break;
      case WHILE:
          printf("<PR, WHILE> ");
          break;
      case ENDW:
          printf("<PR, ENDW> ");
          break;
      case VAR:
          printf("<PR, VAR> ");
          break;
      case FROM:
          printf("<PR, FROM> ");
          break;
      case TO:
          printf("<PR, TO> ");
          break;
      case DT:
          printf("<PR, DT> ");
          break;
      case BY:
          printf("<PR, BY> ");
          break;
      case IF:
          printf("<PR, IF> ");
          break;
      case ENDV:
          printf("<PR, ENDV> ");
          break;
      case ELSE:
          printf("<PR, ELSE> ");
          break;
      case ELIF:
          printf("<PR, ELIF> ");
          break;
      case ENDI:
          printf("<PR, ENDI> ");
          break;
      case GETOUT:
          printf("<PR, GETOUT> ");
          break;
      case GETINT:
          printf("<PR, GETINT> ");
          break;
      case GETREAL:
          printf("<PR, GETREAL> ");
          break;
      case GETCHAR:
          printf("<PR, GETCHAR> ");
          break;
      case PUTINT:
          printf("<PR, PUTINT> ");
          break;
      case PUTREAL:
          printf("<PR, PUTREAL> ");
          break;
      case PUTCHAR:
          printf("<PR, PUTCHAR> ");
          break;
      case PUTSTR:
          printf("<PR, PUTSTR> ");
          break;
      case GETSTR:
          printf("<PR, GETSTR> ");
          break;
      case PROT:
          printf("<PR, PROT> ");
          break;
      }
      break;
    case SN:
      switch (tk.codigo) {
      case ADICAO:
        printf("<SN, ADICAO> ");
        break;
      case SUBTRACAO:
        printf("<SN, SUBTRACAO> ");
        break;
      case MULTIPLICACAO:
        printf("<SN, MULTIPLICACAO> ");
        break;
      case DIVISAO:
        printf("<SN, DIVISAO> ");
        break;
      case MENOR:
        printf("<SN, MENOR> ");
        break;
      case MENOR_IGUAL:
        printf("<SN, MENOR_IGUAL> ");
        break;
      case MAIOR:
        printf("<SN, MAIOR> ");
        break;
      case MAIOR_IGUAL:
        printf("<SN, MAIOR_IGUAL> ");
        break;
      case ATRIBUICAO:
        printf("<SN, ATRIBUICAO> ");
        break;
      case IGUALDADE:
        printf("<SN, IGUALDADE> ");
        break;
      case COND_NEGACAO:
        printf("<SN, COND_NEGACAO> ");
        break;
      case DIFERENTE:
        printf("<SN, DIFERENTE> ");
        break;
      case ABRE_PAR:
        printf("<SN, ABRE_PARENTESES> ");
        break;
      case FECHA_PAR:
        printf("<SN, FECHA_PARENTESES> ");
        break;
      case ABRE_COL:
        printf("<SN, ABRE_COLCHETE> ");
        break;
      case FECHA_COL:
        printf("<SN, FECHA_COLCHETE> ");
        break;
      case ABRE_CHAVES:
        printf("<SN, > ABRE_CHAVES");
        break;
      case FECHA_CHAVES:
        printf("<SN, > FECHA_CHAVES");
        break;
      case ENDERECO:
        printf("<SN, ENDERECO> ");
        break;
      case COND_ADICAO:
        printf("<SN, COND_ADICAO> ");
        break;
      case COND_ALTERNATIVA:
        printf("<SN, COND_ALTERNATIVA> ");
        break;
      case VIRGULA:
        printf("<SN, VIRGULA> ");
        break;
      }
      break;

    case CT_I:
      printf("<CT_I, %d> ", tk.valInt);
      break;
    case CT_R:
      printf("<CT_R, %f> ", tk.valFloat);
      break;
    case CT_C:
      printf("<CT_C, %s> ", tk.lexema);
      break;
    case CT_STR:
      printf("<CT_STR, %s> ", tk.lexema);
      break;
    case ERRO:
      printf("<ERRO> #LINHA COM EXPRESSAO INVALIDA#");
      printf("\n");
      break;
    case FIM_EXPR:
      ++contLinha;
      printf("\n");
      break;
    case FIM_ARQ:
      printf("\nFim do arquivo fonte\n");
      fclose(fd);
    }
    break;
  }
}
