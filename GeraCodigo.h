#ifndef GeraCodigo
#define GeraCodigo

#include <stdio.h>
#include <stdbool.h>
#include "Analex.h"
#include "Anasint.h"
#include "Tabela_SINAIS.h"

FILE *pilha;

char* criarLabel();
void escrevePilha(const char* , ...);
void escreveOperacoes(int);
void escreveCodCompara(int);
void escreveConst(TOKEN);
void escreveGetsCmd(int , int );
char* concatenaString(char* , char* );

#endif