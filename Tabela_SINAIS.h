#ifndef TABELA_SINAIS
#define TABELA_SINAIS

#include <stdio.h>
#include <stdbool.h>
#include "Analex.h"
#include "Anasint.h"

#define _GREEN_ "\x1b[97;102m"

#define TAM_MAX_TABELA 100

//declaracao dos enums

typedef enum {
    GLB,
    LCL
}Escopo;

typedef enum {
    N_A_Tipo,
    INT_Tipo = 7,     
    REAL_Tipo = 8,    
    CHAR_Tipo = 6,    
    BOOL_Tipo = 9  
} Tipo;

typedef enum {
    GLOBAL,
    LOCAL,
    PROCED = 3,
    PARAMETRO,
    PROTOTIPO = 2
} Categoria;

typedef enum {
    N_A_Pass,
    VALOR,
    REFERENCIA
} Passagem;

typedef enum {
    N_A_Zumb,
    VIVO,
    ZUMBI
} Zumbi;

typedef enum {
    N_A_Array,
    SIMPLES,
    VETOR,
    MATRIZ
} Array;

typedef enum {
    NAO,
    SIM
} EhConst;

// struct da tabela

typedef union {
    int valor_int;
    float valor_real;
    char valor_char[100];
    bool valor_bool;
} ValorConst;

typedef struct {
    char lexema[TAM_MAX_LEXEMA];
    Escopo escopo;
    Tipo tipo;
    Categoria categoria;
    Passagem passagem;
    Zumbi zumbi;  
    Array array;           
    int dim1; 
    int dim2; 
    EhConst eh_const; 
    ValorConst valor_const;
    int endereco; 
    char rotulo[TAM_MAX_LEXEMA];
} SIMBOLO;

// Variaveis Globais
extern SIMBOLO tabela_simbolos[TAM_MAX_TABELA];
extern int TOPO;
extern int contLinha;

// Funcoes
int Insere_Tabela(const char*, Escopo);
int Consulta_Tabela(const char*, int);
void Insere_Tabela_decl_var_escalar(int, Tipo, Categoria, Array, EhConst);
void Insere_Valor(int , TOKEN , int , int []);
void Insere_Tabela_simb_decl_var_array(int, Tipo, Categoria, int, int[], EhConst);
void Insere_Tabela_decl_def_prot(const char *, Escopo, Categoria);
void Insere_Tabela_parametro(Escopo, Tipo, Categoria, Passagem, int);
int Insere_Tabela_parametro_procedimento(const char*, int);

void Verifica_Tabela_parametro(int, Escopo, Tipo, Categoria, Passagem, int);
void Veri_Quant_param_maior(int , int );
void Veri_Quant_param_menor(int, int);
//int Veri_Tipo(int , TOKEN );
int Veri_Tipo(int , int );

void Remove_Tabela();
void TornarVivo(TOKEN);
void TornarZumbi(TOKEN);
void Imprimi_Tabela();

#endif