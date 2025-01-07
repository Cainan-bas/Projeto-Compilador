#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include "Analex.h"
#include "Anasint.h"
#include "Tabela_SINAIS.h"

FILE *pilha;

char *criarLabel(){
    static int count = 1;
    static char label[8];
    snprintf(label, sizeof(label),"L%d",count);
    count++;
    return label;
}

// void escrevePilha(char* frase){
//     fputs(frase, pilha);
//     fputc('\n', pilha);
// }

void escrevePilha(const char* frase, ...) {
    va_list aux;                    
    char vetor_aux[256];                
    va_start(aux, frase);          
    vsnprintf(vetor_aux, sizeof(vetor_aux), frase, aux); 
    va_end(aux);                    
    fputs(vetor_aux, pilha);            
}

void escreveOperacoes(int codigo) {
    if (codigo == ADICAO) escrevePilha("ADD\n");
    else if (codigo == SUBTRACAO) escrevePilha("SUB\n");
    else if (codigo == MULTIPLICACAO) escrevePilha("MULT\n");
    else if (codigo == DIVISAO) escrevePilha("DIV\n");
}

void escreveConst(TOKEN valor){
    if (valor.codigo == CT_I) escrevePilha("PUSH %d\n", valor.valInt);
    else if (valor.codigo == CT_R) escrevePilha("PUSHF %f\n", valor.valFloat);
    else if (valor.codigo == CT_C) escrevePilha("PUSH %d\n", valor.lexema);
}

void escreveCodCompara(int codigo){
    char label01[10], label02[10], label03[10];

    escrevePilha("SUB\n");
    strcpy(label01, criarLabel());
    strcpy(label02, criarLabel());

    if(codigo == IGUALDADE){
        escrevePilha("GOFALSE %s\nPUSH 0\nGOTO %s\nLABEL %s\nPUSH 1\nLABEL %s\n", label01,label02,label01,label02);
    } else if(codigo == DIFERENTE){
        escrevePilha("GOFALSE %s\nPUSH 1\nGOTO %s\nLABEL %s\nPUSH 0\nLABEL %s\n", label01,label02,label01,label02);
    } else if(codigo == MENOR){
        strcpy(label03, criarLabel());
        escrevePilha("COPY \nGOTRUE %s\nGOFALSE %s\nPUSH 1\nGOTO %s\nLABEL %s\nPOP \nLABEL %s\nPUSH 0\nLABEL %s\n", label01,label02,label03,label01,label02,label03);
    } else if(codigo == MENOR_IGUAL){
        escrevePilha("GOTRUE %s\nPUSH 1\nGOTO %s\nLABEL %s\nPUSH 0\nLABEL %s\n", label01,label02,label01,label02);
    } else if(codigo == MAIOR){
        escrevePilha("GOTRUE %s\nPUSH 0\nGOTO %s\nLABEL %s\nPUSH 1\nLABEL %s\n", label01,label02,label01,label02);
    } else if(codigo == MAIOR_IGUAL){
        strcpy(label03, criarLabel());
        escrevePilha("COPY \nGOFALSE %s\nGOTRUE %s\nPUSH 0\nGOTO %s\nLABEL %s\nPOP \nLABEL %s\nPUSH 1\nLABEL %s\n", label01,label02,label03,label01,label02,label03);
    }
}

void escreveGetsCmd(int codigo, int posicao){
    if (codigo == GETINT) escrevePilha("GET_I\n");
    else if (codigo == GETREAL) escrevePilha("GET_F\n");
    else if (codigo == GETCHAR) escrevePilha("GET_C\n");

    escrevePilha("LOAL %d, %d\n", tabela_simbolos[posicao].escopo, tabela_simbolos[posicao].endereco);
}

char* concatenaString(char* destino, char* final) {

    if (destino == NULL || final == NULL) return NULL; 

    strcat(destino, final); 
    return destino;         
}
