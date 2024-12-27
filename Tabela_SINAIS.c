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
    // case GLB: return "GLB";
    // case LCL: return "LCL";
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
        case GLOBAL: return "VA_GLOBAL";
        case LOCAL: return "VA_LOCAL";
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


// int Consulta_Tabela(const char *lexema, int num) {
//     for (int i = num; i < TOPO; i++) {
//         if (strcmp(tabela_simbolos[i].lexema, lexema) == 0) {
//             return i;
//         }
//     }
//     return -1;
// }

int Consulta_Tabela(const char *lexema, int num) {
    if(num >= 0){
        for (int i = num; i < TOPO; i++) {
            if (strcmp(tabela_simbolos[i].lexema, lexema) == 0) {
                return i;
            }
        }
    } else {
        for (int i = TOPO-1; i >= 0; i--) {
            if (strcmp(tabela_simbolos[i].lexema, lexema) == 0) {
                return i;
            }
        }
    }
    return -1;
}

int Insere_Tabela(const char *lexema, Escopo escopo) {
    int posicaoLocal = 0;
    // melhorar essa consulta na tabela, talvez colocar por token
    posicaoLocal = Consulta_Tabela(lexema, 0);
    if (posicaoLocal != -1) {
        if(tabela_simbolos[posicaoLocal].categoria != GLOBAL) error("Redeclaracao de variavel LOCAL");
        posicaoLocal = Consulta_Tabela(lexema, posicaoLocal);
        if(posicaoLocal != -1){
            if(tabela_simbolos[posicaoLocal].categoria == GLOBAL) error("Redeclaracao de variavel GLOBAL");
            // printf("Erro: Símbolo '%s' já declarado.\n", lexema);
            // return -1;
        }
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

    // tabela_simbolos[TOPO].endereco = TOPO;
    Imprimi_Tabela();
}

void Insere_Valor(int k, TOKEN valor, int cont_dim, int tam_dims[]){
    // Insere valor dimensao vetor/matriz
    if(tabela_simbolos[k].array == VETOR || tabela_simbolos[k].array == MATRIZ){
        tabela_simbolos[k].dim1 = (cont_dim >= 1) ? tam_dims[0] : 0;
        tabela_simbolos[k].dim2 = (cont_dim == 2) ? tam_dims[1] : 0;
    }else{
        // Insere valor const
        // if(valor.cat == CT_I && tabela_simbolos[k].tipo == INT_Tipo){
        if(valor.cat == CT_I && (tabela_simbolos[k].tipo == INT_Tipo || tabela_simbolos[k].tipo == CHAR_Tipo)){
            tabela_simbolos[k].valor_const.valor_int = valor.valInt;    
        } else if (valor.cat == CT_R && tabela_simbolos[k].tipo == REAL_Tipo){
            tabela_simbolos[k].valor_const.valor_real = valor.valFloat;
        // } else if (valor.cat == CT_C && tabela_simbolos[k].tipo == CHAR_Tipo){
        } else if (valor.cat == CT_C && (tabela_simbolos[k].tipo == CHAR_Tipo || tabela_simbolos[k].tipo == INT_Tipo)){
            strcpy(tabela_simbolos[k].valor_const.valor_char, valor.lexema);
        } else if (valor.cat == CT_I && tabela_simbolos[k].tipo == BOOL_Tipo){
            tabela_simbolos[k].valor_const.valor_bool = (valor.valInt == 0 ? 0 : 1);
        } else {
            error("Atribuicao de tipo invalida");
        }
    }
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
    int posicaoLocal = 0;
    posicaoLocal = Consulta_Tabela(lexema, 0);

    if (posicaoLocal != -1) {
        // if(tabela_simbolos[posicaoLocal].categoria != PROT) error("Variavel local/global com mesmo nome que PROT/PROC");
        if(categoria == PROT) error("Variavel local/global com mesmo nome que PROT/PROC");
        if(categoria == tabela_simbolos[posicaoLocal].categoria) error("Redeclaracao de procedimento");
        tabela_simbolos[posicaoLocal].categoria = categoria;
        Imprimi_Tabela();
    } else {
        strncpy(tabela_simbolos[TOPO].lexema, lexema, TAM_MAX_LEXEMA);
        tabela_simbolos[TOPO].escopo = escopo;
        // tabela_simbolos[TOPO].categoria = categoria;
        //pequena mudança
        Insere_Tabela_decl_var_escalar(TOPO, N_A_Tipo,  categoria,  N_A_Array,  NAO);

        Imprimi_Tabela();
        tabela_simbolos[TOPO].endereco = TOPO;
        TOPO++;
    }
    // return TOPO-1;
}

void Verifica_Tabela_parametro(int k, Escopo escopo, Tipo tipo, Categoria categoria, Passagem passagem, int cont_dim){

    if(tabela_simbolos[k].passagem != passagem) error("Parametro com passagem invalita");
    if(tabela_simbolos[k].tipo != tipo) error("Parametro com tipo invalido");
    if(cont_dim == 0){
        if(tabela_simbolos[k].array != SIMPLES) error("Parametro com array de tamanho invalido");
    } else if (cont_dim == 1){
        if(tabela_simbolos[k].array != VETOR) error("Parametro com array de tamanho invalido");
    } else if (cont_dim == 2){
        if(tabela_simbolos[k].array != MATRIZ) error("Parametro com array de tamanho invalido");
    } else{
        error("Mais colchetes que o permitido");
    }
}

void Veri_Quant_param_maior(int topoLocal, int tamanPara){
    int quantMax = 0;
    while (tabela_simbolos[topoLocal+1].categoria == PARAMETRO){
        quantMax++;
        topoLocal++;
    }
    if(tamanPara > quantMax) error("Mais parametros na declaracao do PROC do que no PROT"); 
}

void Veri_Quant_param_menor(int topoLocal, int tamanPara){
     int quantMax = 0;
    while (tabela_simbolos[topoLocal+1].categoria == PARAMETRO){
        quantMax++;
        topoLocal++;
    }
    if(tamanPara < quantMax) error("Falta parametros na declaracao do PROC do que no PROT");
}

//recebe dois valores e compara
// ainda em andamento
int Veri_Tipo(int tipoComp, int valor){
    if((valor == CT_I || valor == INT_Tipo) && (tipoComp == INT_Tipo || tipoComp == CHAR_Tipo || tipoComp == CT_I || tipoComp == CT_C)){
        return INT_Tipo;    
    } else if ((valor == CT_R || valor == REAL_Tipo) && (tipoComp == REAL_Tipo || tipoComp == CT_R)){ 
        return REAL_Tipo;
    } else if ((valor == CT_C || valor == CHAR_Tipo) && (tipoComp == CHAR_Tipo || tipoComp == INT_Tipo || tipoComp == CT_I || tipoComp == CT_C)){
        return INT_Tipo;
    } else if ((valor == CT_I || valor == INT_Tipo || valor == BOOL_Tipo) && (tipoComp == BOOL_Tipo || tipoComp == CT_I)){
        return INT_Tipo;
    } else {
        return -1;
    }
}

void Insere_Tabela_parametro(Escopo escopo, Tipo tipo, Categoria categoria, Passagem passagem, int cont_dim) {
    tabela_simbolos[TOPO].escopo = escopo;
    tabela_simbolos[TOPO].tipo = tipo;
    tabela_simbolos[TOPO].passagem = passagem;
    tabela_simbolos[TOPO].categoria = categoria;
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
}

int Insere_Tabela_parametro_procedimento(const char* lexema, int topoLocal){
    
    strncpy(tabela_simbolos[topoLocal].lexema, lexema, TAM_MAX_LEXEMA);
    tabela_simbolos[topoLocal].zumbi = ZUMBI;
    if(tabela_simbolos[topoLocal].categoria == PARAMETRO){
        Imprimi_Tabela();
        return topoLocal;
    }
    Imprimi_Tabela();
    return -1;
}

void Remove_Tabela(){
    // topo sempre aponta pra nada, a ultima posicao e TOPO - 1
    while (TOPO >= 0) {
        if (tabela_simbolos[TOPO-1].categoria == PARAMETRO || tabela_simbolos[TOPO-1].categoria == PROCED || tabela_simbolos[TOPO-1].categoria == PROTOTIPO) {
            break;
        }
        TOPO--;
        Imprimi_Tabela();
    }
}

void TornarVivo(TOKEN nomeDef){
    int posicao = Consulta_Tabela(nomeDef.lexema, 0);
    while(posicao <= TOPO){
        posicao+=1;
        if (tabela_simbolos[posicao].categoria != PARAMETRO) break;
        tabela_simbolos[posicao].zumbi = VIVO;
        // Imprimi_Tabela();
    }
}

void TornarZumbi(TOKEN nomeDef){
    int posicao = Consulta_Tabela(nomeDef.lexema, 0);
    while(posicao <= TOPO){
        posicao+=1;
        if (tabela_simbolos[posicao].categoria != PARAMETRO) break;
        tabela_simbolos[posicao].zumbi = ZUMBI;
        // Imprimi_Tabela();
    }
}

void Imprimi_Tabela() {
    printf("\n");
    printf("  TOPO  ->  %d\n", TOPO);
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
                printf("%-9s │ ", sim.valor_const.valor_char);
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