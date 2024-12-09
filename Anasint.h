#ifndef _ANASINT_
#define _ANASINT_

#include <stdio.h>
#include <stdbool.h>
#include "Analex.h"

#define MAX_ARRAY_DIM 100


/* Variaveis globais */
extern TOKEN t;
extern FILE *fd;
extern int contLinha;

/* Assinaturas das funcoes */

void Prog();
void Decl_list_var();
void Decl_var(int, bool);
void Decl_def_prot();
void Func_CMD();
void Func_Atrib();
void Func_Expr();
void Func_ExprSimples();
void Func_Termo();
void Func_Fator();

#endif // ANASINT