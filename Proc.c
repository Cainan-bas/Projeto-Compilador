#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "Analex.h"
#include "Anasint.h"
// #include "Funcaux.h"

/* Variaveis globais */
TOKEN t;
FILE *fd;
int contLinha;
char TABS[200] = "";

void TestaAnalex(){
    if ((fd=fopen("teste.txt", "r")) == NULL)
        error("Arquivo de entrada da expressão nao encontrado!");
    
    t.processado = true; // Garante que o próximo token será lido
    while (true) {
        t = Analex(fd);
        if (t.cat == ERRO){
            printf("\nLINHA %d: ", contLinha);
            printf("Erro lexico ");
            break;
        }
        if (t.cat == FIM_ARQ) {
            printf("\nLINHA %d: ", contLinha);
            printf("Fim do arquivo fonte encontrado!\n");
            break;
        }
        if (t.cat == FIM_ARQ) break;
        t.processado = true;
    }
    fclose(fd);
}

void TestaAnasint() {
    
    if ((fd=fopen("teste.txt", "r")) == NULL)
        error("Arquivo de entrada da expressão nao encontrado!");
    
    t.processado = true; 
    while (true) {
        t = analex();
        if (t.cat == FIM_ARQ) {
            printf("\nFim do arquivo fonte encontrado!\n");
            break;
        }
        Prog();
        // if (t.cat==FIM_EXPR)
        //     printf("\nLINHA %d: Expressão sintaticamente correta!\n\n", contLinha - 1);
        if (t.cat==FIM_ARQ)
            printf("\nLINHA %d: Expressão sintaticamente correta!\n\n", contLinha - 1);
        else {
            error("Erro de sintaxe!");
        }
        t.processado = true;
    }
    fclose(fd);
}

int main() {

    system("chcp 65001");  // Troca a página de códigos de caracteres 
                           // da console para UTF-8 na execução

    contLinha = 1;
    printf("\n\n[Análise Léxica -------------------]\n");
    TestaAnalex();

    contLinha = 1;
    printf("\n\n[Análise Sintática ----------------]\n");
    TestaAnasint();

}