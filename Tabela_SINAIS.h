// #ifndef TABELA_SINAIS
// #define TABELA_SINAIS

// #include <stdio.h>
// #include <stdbool.h>
// #include "Analex.h"
// #include "Anasint.h"

// #define TAM_MAX_TABELA 100

// // struct da tabela

// typedef union {
//     int valor_int;
//     float valor_real;
//     char valor_char;
//     bool valor_bool;
// } ValorConst;

// typedef struct {
//     char lexema[TAM_MAX_LEXEMA];
//     int escopo;
//     Tipo tipo;
//     Categoria categoria;
//     Passagem passagem;
//     Zumbi zumbi;  
//     Array array;           
//     int dim1; 
//     int dim2; 
//     EhConst eh_const; 
//     ValorConst valor_const;
//     int endereco; 
//     int rotulo;
// } SIMBOLO;

// //declaracao dos enums

// typedef enum {
//     N_A,
//     INT,     
//     REAL,    
//     CHAR,    
//     BOOL   
// } Tipo;

// typedef enum {
//     GLOBAL,
//     LOCAL,
//     PROCED,
//     PARAMETRO,
//     PROTOTIPO
// } Categoria;

// typedef enum {
//     N_A,
//     VALOR,
//     REFE
// } Passagem;

// typedef enum {
//     N_A,
//     VIVO,
//     ZUMBI
// } Zumbi;

// typedef enum {
//     N_A,
//     SIMPLES,
//     VETOR,
//     MATRIZ
// } Array;

// typedef enum {
//     NAO,
//     SIM
// } EhConst;

// // Variaveis Globais
// extern SIMBOLO tabela_simbolos[TAM_MAX_TABELA];
// extern int TOPO = 0;

// // Funcoes
// void Insere_Tabela(TOKEN, int);
// void Consulta_Tabela();
// void Remove_Tabela();

// #endif