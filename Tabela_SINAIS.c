#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Analex.h"
#include "Anasint.h"
#include "Tabela_SINAIS.h"

int TOPO = 0;
SIMBOLO tabela_simbolos[TAM_MAX_TABELA];

// -----------------------------------------------------------------------
const char* escopoToString(Escopo escopo){
    switch (escopo)
    {
    case GLB: return "Externo";
    case LCL: return "Interno";
    default: return "UNKN";
    }
}
const char* tipoToString(Tipo tipo) {
    switch (tipo) {
        case INT_Tipo: return "INT";
        case REAL_Tipo: return "REAL";
        case CHAR_Tipo: return "CHAR";
        case BOOL_Tipo: return "BOOL";
        default: return "N/A"; // testar esse UNKNOWN
    }
}
const char* categoriaToString(Categoria categoria) {
    switch (categoria) {
        case GLOBAL: return "GLOBAL";
        case LOCAL: return "LOCAL";
        case PROCED: return "PROCED";
        case PARAMETRO: return "PARAMETRO";
        case PROTOTIPO: return "PROTOTIPO";
        default: return "UNKNOWN"; // testar esse UNKNOWN
    }
}
const char* passagemToString(Passagem passagem) {
    switch (passagem) {
        case VALOR: return "VALOR";
        case REFERENCIA: return "REFERENCIA";
        default: return "N/A"; // testar esse UNKNOWN
    }
}
const char* zumbiToString(Zumbi zumbi) {
    switch (zumbi) {
        // case N_A_Zumb: return "N/A";
        case VIVO: return "VIVO";
        case ZUMBI: return "ZUMBI";
        default: return "N/A"; // testar esse UNKNOWN
    }
}
const char* arrayToString(Array array) {
    switch (array) {
        case SIMPLES: return "SIMPLES";
        case VETOR: return "VETOR";
        case MATRIZ: return "MATRIZ";
        default: return "N/A"; // testar esse UNKNOWN
    }
}
const char* ehConstToString(EhConst eh_const) {
    return eh_const == SIM ? "SIM" : "NAO";
}


int Consulta_Tabela(const char *lexema) {
    for (int i = 0; i < TOPO; i++) {
        if (strcmp(tabela_simbolos[i].lexema, lexema) == 0) {
            return i;
        }
    }
    return -1;
}

int Insere_Tabela(const char *lexema, Escopo escopo) {
    
    if (Consulta_Tabela(lexema) != -1) {
        printf("Erro: Símbolo '%s' já declarado.\n", lexema);
        return -1;
    }

    strncpy(tabela_simbolos[TOPO].lexema, lexema, TAM_MAX_LEXEMA);
    tabela_simbolos[TOPO].escopo = escopo;
    
    tabela_simbolos[TOPO].endereco = TOPO;
    // Imprimi_Tabela();

    TOPO++;
    return TOPO-1;
}

void Insere_Tabela_decl_var_escalar(int k, Tipo tipo, Categoria categoria, Array array, EhConst eh_const) {

    // Inicializa o símbolo
    tabela_simbolos[k].tipo = tipo;
    tabela_simbolos[k].categoria = categoria;
    tabela_simbolos[k].passagem = N_A_Pass;
    tabela_simbolos[k].zumbi = N_A_Zumb;
    tabela_simbolos[k].array = array;
    tabela_simbolos[k].dim1 = 0;
    tabela_simbolos[k].dim2 = 0;
    tabela_simbolos[k].eh_const = eh_const;

    tabela_simbolos[TOPO].endereco = TOPO;
    Imprimi_Tabela();
}

void Insere_Tabela_simb_decl_var_array(int k, Tipo tipo, Categoria categoria, int cont_dim, int tam_dims[], EhConst eh_const) {

    // Inicializa o símbolo
    tabela_simbolos[k].tipo = tipo;
    tabela_simbolos[k].categoria = categoria;
    tabela_simbolos[k].passagem = N_A_Pass;
    tabela_simbolos[k].zumbi = N_A_Zumb;
    tabela_simbolos[k].array = (cont_dim == 1) ? VETOR : MATRIZ;
    tabela_simbolos[k].dim1 = (cont_dim >= 1) ? tam_dims[0] : 0;
    tabela_simbolos[k].dim2 = (cont_dim == 2) ? tam_dims[1] : 0;
    tabela_simbolos[k].eh_const = eh_const;

    tabela_simbolos[TOPO].endereco = TOPO;
    Imprimi_Tabela();
}

void Insere_Tabela_decl_def_prot(const char *lexema, Escopo escopo, Categoria categoria) {
    
    if (Consulta_Tabela(lexema) != -1) {
        tabela_simbolos[Consulta_Tabela(lexema)].categoria = categoria;
        Imprimi_Tabela();
    } else {
        strncpy(tabela_simbolos[TOPO].lexema, lexema, TAM_MAX_LEXEMA);
        tabela_simbolos[TOPO].escopo = escopo;
        tabela_simbolos[TOPO].categoria = categoria;

        Imprimi_Tabela();
        tabela_simbolos[TOPO].endereco = TOPO;
        TOPO++;
    }
    // return TOPO-1;
}

void Insere_Tabela_parametro(Escopo escopo, Tipo tipo, Categoria categoria, Passagem passagem, int cont_dim) {

    tabela_simbolos[TOPO].escopo = escopo;
    tabela_simbolos[TOPO].tipo = tipo;
    tabela_simbolos[TOPO].categoria = categoria;
    tabela_simbolos[TOPO].passagem = passagem;
    if(cont_dim == 0){
        tabela_simbolos[TOPO].array = SIMPLES;
    } else if (cont_dim == 1){
        tabela_simbolos[TOPO].array = VETOR;
    } else if (cont_dim == 2){
        tabela_simbolos[TOPO].array = MATRIZ;
    } else{
        error("Mais colchetes que o permitido");
    }
    tabela_simbolos[TOPO].endereco = TOPO;
    Imprimi_Tabela();
    
    TOPO++;
    // return TOPO-1;
}

int Insere_Tabela_parametro_procedimento(const char* lexema, int topoLocal){
    strncpy(tabela_simbolos[topoLocal].lexema, lexema, TAM_MAX_LEXEMA);
    tabela_simbolos[topoLocal].zumbi = ZUMBI;
    if(tabela_simbolos[topoLocal+1].categoria != PARAMETRO){
        Imprimi_Tabela();
        return -1;
    }
    Imprimi_Tabela();
    return topoLocal;
}

void Remove_Tabela(){
    // topo sempre aponta pra nada, a ultima posicao e TOPO - 1
    while (TOPO >= 0) {
        if (tabela_simbolos[TOPO-1].categoria == PARAMETRO) {
            break;
        }
        TOPO--;
        Imprimi_Tabela();
    }
}

void Imprimi_Tabela() {
    printf("\n");
    printf("┌───────────────────────────────┬──────┬────────┬───────────┬────────────┬─────────┬─────────┬────────┬────────┬───────────┬───────────┬──────────┐\n");
    printf("│           Lexema              │ Tipo │ Escopo │ Categoria │  Passagem  │  Zumbi  │  Array  │ Dim. 1 │ Dim. 2 │ EhConst   │ ValorConst│ Endereço │\n");
    printf("├───────────────────────────────┼──────┼────────┼───────────┼────────────┼─────────┼─────────┼────────┼────────┼───────────┼───────────┼──────────┤\n");

    for (int i = 0; i < TOPO; i++) {
        SIMBOLO sim = tabela_simbolos[i];

        printf("│ %-29s │ %-4s │%-6s │ %-9s │ %-10s │ %-7s │ %-7s │ %-6d │ %-6d │ %-9s │ ", 
               sim.lexema, 
               tipoToString(sim.tipo), 
               escopoToString(sim.escopo), 
               categoriaToString(sim.categoria), 
               passagemToString(sim.passagem), 
               zumbiToString(sim.zumbi), 
               arrayToString(sim.array), 
               sim.dim1, 
               sim.dim2, 
               ehConstToString(sim.eh_const));

        // Exibe o valor constante dependendo do tipo
        switch (sim.tipo) {
            case INT_Tipo:
                printf("%-9d │ ", sim.valor_const.valor_int);
                break;
            case REAL_Tipo:
                printf("%-8.2f │ ", sim.valor_const.valor_real);
                break;
            case CHAR_Tipo:
                printf("%-9c │ ", sim.valor_const.valor_char);
                break;
            case BOOL_Tipo:
                printf("%-9s │ ", sim.valor_const.valor_bool ? "true" : "false");
                break;
            default:
                printf("    N/A   │ ");
                break;
        }
        printf("%-8d │\n", sim.endereco);

        // Adiciona uma linha divisória entre as entradas
        if (i < TOPO - 1) {
            printf("├───────────────────────────────┼──────┼────────┼───────────┼────────────┼─────────┼─────────┼────────┼────────┼───────────┼───────────┼──────────┤\n");
        }
    }

    printf("└───────────────────────────────┴──────┴────────┴───────────┴────────────┴─────────┴─────────┴────────┴────────┴───────────┴───────────┴──────────┘\n");
    printf("Pressione Enter para continuar...\n");
    getchar();
}