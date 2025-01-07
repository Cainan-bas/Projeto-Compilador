#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Analex.h"
#include "Anasint.h"
#include "Tabela_SINAIS.h" 
#include "GeraCodigo.h"


/* Variaveis globais */
TOKEN t;
FILE *fd;
TOKEN aux;

// variaveis da tabela
int escopo_atual;
bool testa_const;
int tipo;
bool testa_array, testa_matriz;
int tam_dims[MAX_ARRAY_DIM];
int endeDeclVar;

int tipoVerificado;

TOKEN analex(){
    do{
        t = Analex(fd);
        // Print_Analex(t);
        if(t.cat == ERRO) error("Erro lexico encontrado");
    }while(t.cat == FIM_EXPR);
    return t;
}

void printError(TOKEN aux){
    printf("------------------------------\n");
    printf("Categoria: %d\n", t.cat);
    printf("Codigo: %d\n", t.codigo);
    printf("Lexema: %s\n", t.lexema);
    printf("Pressione Enter para continuar...\n");
    getchar();
    printf("------------------------------\n");
    
}

void Prog() {
    escopo_atual = GLB;
    escrevePilha("INIP\n");
    // t = analex();
    endeDeclVar = 0;
    while ((t.cat == PR) &&
           ((t.codigo == CONST) || (t.codigo == INT) || (t.codigo == REAL) ||
            (t.codigo == CHAR) || (t.codigo == BOOL))) {
        Decl_list_var();
    }
    while ((t.cat == PR) && ((t.codigo == PROT) || (t.codigo == DEF))) {
        escopo_atual = GLB;
        Decl_def_prot();
    }

   for (int i = 0; i < TOPO; i++) {
        if (tabela_simbolos[i].categoria == PROT) {
            error("PROT sem definicao encontrado");
        }
    }
    
    if (t.cat != FIM_ARQ) error("Declaração ou definição de procedimento esperado!");
    escrevePilha("HALT\n");
}

void Decl_list_var(){

    testa_const = ((t.cat == PR) && (t.codigo == CONST)); //testa o const

    if (testa_const) {
        t.processado = true;
        t = analex();
    }

    if (!((t.cat == PR) && ((t.codigo == INT) || (t.codigo == REAL) ||
        (t.codigo == CHAR) || (t.codigo == BOOL)))) error("Tipo esperado.");

    tipo = t.codigo;
    t.processado = true;
    t = analex();
    
    Decl_var();
    printf("endereco %d, linha - %d\n", endeDeclVar, contLinha);

    while ((t.cat == SN) && (t.codigo == VIRGULA)) {
        t.processado = true;
        t = analex();
        Decl_var();
        printf("endereco %d, linha - %d\n", endeDeclVar, contLinha);
    }
}

void Decl_var(){
    int topoLocal;
    int posicao;
    int cont_dim = 0;

    if(t.cat != ID) error("Identificador esperado");
    topoLocal = Insere_Tabela(t.lexema, escopo_atual);
    t.processado = true;
    t = analex();

    testa_array = ((t.cat == SN) && (t.codigo == ABRE_COL));
    if(testa_array){
        while ((t.cat == SN) && (t.codigo == ABRE_COL)){
            cont_dim++;
            if(cont_dim>2) error("Definicao maior que matriz encontrada");
            t.processado = true;
            t = analex();
            if(t.cat == CT_I){
                tam_dims[cont_dim - 1] = t.valInt;
                t.processado = true;
                t = analex();
            } else if (t.cat == ID){
                posicao = Consulta_Tabela(t.lexema, 0);
                if (!(posicao != -1)) error("Identificador de matriz/vetor,nao encontrado");
                if(!(tabela_simbolos[posicao].tipo == INT_Tipo && tabela_simbolos[posicao].eh_const == SIM)) error("parametro de array/matriz diferente de inteiro ou constante inteira");
                tam_dims[cont_dim - 1] = tabela_simbolos[posicao].valor_const.valor_int;
                t.processado = true;
                t = analex();
            } else {
                error ("Dimensão de array identificador constante inteiro ou constante inteira esperada");
            }
            if((t.cat != SN) && (t.codigo != FECHA_COL)) error ("Fecha colchete esperado");
            t.processado = true;
            t = analex();
            // break;
        }
        Insere_Tabela_simb_decl_var_array(topoLocal, tipo, (escopo_atual == GLB ? GLOBAL : LOCAL), cont_dim, tam_dims, testa_const);
    }else{
        Insere_Tabela_decl_var_escalar(topoLocal, tipo, (escopo_atual == GLB ? GLOBAL : LOCAL), SIMPLES, testa_const);
    }

    if(t.cat == SN && t.codigo == ATRIBUICAO){
        
        t.processado = true;
        t = analex();
        if(testa_array){
            if(!(t.cat == SN && t.codigo == ABRE_CHAVES)) error("Incialização de array inválida");
            t.processado = true;
            t = analex();
            while(1){
                t.processado = true;
                t = analex();
                if(t.cat == SN && t.codigo == VIRGULA){
                    t.processado = true;
                    t = analex();
                    continue;
                } else if (t.cat == SN && t.codigo == FECHA_CHAVES){
                    t.processado = true;
                    t = analex();
                    break;
                } else {
                    error("Vírgula ou fecha parênteses esperado");
                }
            }
        } else {
            if(t.cat == SN && t.codigo == ABRE_CHAVES) error("Chaves na declaracao de variavel escalar");
            if(testa_const) Insere_Valor(topoLocal, t,0,0); //depois verificar como atribuir os valores nos parametros array
            if(Veri_Tipo(tabela_simbolos[topoLocal].tipo, t.cat) == -1) error("Tipos incompativeis");
            //if(Veri_Tipo(topoLocal, t) == -1) error("Tipos incompativeis");
            t.processado = true;
            t = analex();
        }
    }

}

void Decl_def_prot(){
    TOKEN nomeID;
    int cont_dim = 0;
    int tam_dims[MAX_ARRAY_DIM];
    int protLocal;
    int passagemLocal;
    int topoLocal;
    int tamanPara = 0;

    if(t.cat == PR && t.codigo == PROT){
        t.processado = true;
        protLocal = t.codigo;
        t = analex();   
        
        if(t.cat == PR && t.codigo == INIT) error("INIT nao possue PROT");
        if(t.cat != ID) error("Identificador esperado"); //testa idprot
        t.processado = true;
        Insere_Tabela_decl_def_prot(t.lexema, escopo_atual, (protLocal == PROT ? PROTOTIPO : PROCED));
        escopo_atual = LCL;
        t = analex();

        if(!(t.cat == SN && t.codigo == ABRE_PAR)) error("Incialização de prot inválida, falta parenteses");
        
        endeDeclVar = -3;
        do{
            t.processado = true;
            t = analex();
            
            if(t.cat == SN && t.codigo == FECHA_PAR) break;

            passagemLocal = VALOR;
            if ((t.cat == SN) && (t.codigo == ENDERECO)) {
                t.processado = true;
                passagemLocal = REFERENCIA;
                t = analex();
            }

            if (!((t.cat == PR) && ((t.codigo == INT) || (t.codigo == REAL) ||
            (t.codigo == CHAR) || (t.codigo == BOOL)))) error("Tipo esperado.");

            t.processado = true;
            tipo = t.codigo;
            t = analex();

            while((t.cat == SN) && (t.codigo == ABRE_COL)){
                cont_dim++;
                t.processado = true;
                t = analex();

                if(!(t.cat == SN && t.codigo == FECHA_COL)) error("Fecha colchete esperado");
                t.processado = true;
                t = analex(); 
            }
            Insere_Tabela_parametro(escopo_atual, tipo, PARAMETRO, passagemLocal, cont_dim);

            cont_dim = 0;
        }while(t.cat == SN && t.codigo == VIRGULA);

        if(!(t.cat == SN && t.codigo == FECHA_PAR)) error("Incialização de prot inválida, falta parenteses");
        t.processado = true;
        t = analex();

    } else {
        t.processado = true;
        protLocal = t.codigo;
        t = analex();

        if((t.cat == PR && t.codigo == INIT)){
            t.processado = true;
            Insere_Tabela_decl_def_prot("init", escopo_atual, PROCED);
            
            escrevePilha("LABEL %s\n",tabela_simbolos[Consulta_Tabela("init", 0)].rotulo);
            escrevePilha("INIP 1\n");

            escopo_atual = LCL;
            t = analex();
        }
        else if(t.cat == ID ){
            t.processado = true;
            // Insere_Tabela_decl_def_prot(t.lexema, escopo_atual, (protLocal == PROT ? PROTOTIPO : PROCED));
            Insere_Tabela_decl_def_prot(t.lexema, escopo_atual, PROCED);
            nomeID = t;
            escopo_atual = LCL;
            topoLocal = Consulta_Tabela(t.lexema, 0);
            
            escrevePilha("LABEL %s\n",tabela_simbolos[topoLocal].rotulo);
            escrevePilha("INIP 1\n");

            if(topoLocal+1 == TOPO) topoLocal=-1; //forca topolocal a ser igual -1 para nao entrar na primeira vez no if de PROT
            
            t = analex();
            if(!(t.cat == SN && t.codigo == ABRE_PAR)) error("Incialização de prot inválida, falta parenteses");

            do{
                t.processado = true;
                t = analex();

                if(t.cat == SN && t.codigo == FECHA_PAR) break;

                passagemLocal = VALOR;
                if ((t.cat == SN) && (t.codigo == ENDERECO)) {
                    t.processado = true;
                    passagemLocal = REFERENCIA;
                    t = analex();
                }

                if (!((t.cat == PR) && ((t.codigo == INT) || (t.codigo == REAL) ||
                (t.codigo == CHAR) || (t.codigo == BOOL)))) error("Tipo esperado.");

                t.processado = true;
                tipo = t.codigo;
                t = analex();

                if(t.cat != ID ) error("Identificador esperado"); 
                if(Consulta_Tabela(t.lexema, topoLocal-tamanPara) != -1)error("Parametro como ID repetido");
                t.processado = true;
                
                tamanPara++;

                if(topoLocal != -1){
                    topoLocal += 1;
                    Veri_Quant_param_maior(Consulta_Tabela(nomeID.lexema, 0), tamanPara);
                    topoLocal = Insere_Tabela_parametro_procedimento(t.lexema, topoLocal);
                } else {
                    topoLocal = TOPO;
                    topoLocal = Insere_Tabela_parametro_procedimento(t.lexema, topoLocal);
                }

                t = analex();

                int posicao;
                while((t.cat == SN) && (t.codigo == ABRE_COL)){
                    cont_dim++;
                    if(cont_dim>2) error("Definicao maior que matriz encontrada");
                    t.processado = true;
                    t = analex();

                    if(t.cat == CT_I){
                        tam_dims[cont_dim - 1] = t.valInt;
                        t.processado = true;
                        t = analex();
                    } else if (t.cat == ID){
                        posicao = Consulta_Tabela(t.lexema, 0);
                        if (!(posicao != -1)) error("Identificador de matriz/vetor,nao encontrado");
                        if(!(tabela_simbolos[posicao].tipo == INT_Tipo && tabela_simbolos[posicao].eh_const == SIM)) error("parametro de array/matriz diferente de inteiro");
                        tam_dims[cont_dim - 1] = tabela_simbolos[posicao].valor_const.valor_int;
                        t.processado = true;
                        t = analex();
                    } else {
                        error ("Dimensão de array identificador constante inteiro ou constante inteira esperada");
                    }

                    if(!(t.cat == SN && t.codigo == FECHA_COL)) error("Fecha colchete esperado");
                    t.processado = true;
                    Insere_Valor(topoLocal, t, cont_dim, tam_dims);
                    t = analex(); 
                }

                if(topoLocal == -1){ 
                    Insere_Tabela_parametro(escopo_atual, tipo, PARAMETRO, passagemLocal, cont_dim);
                }else{
                    Verifica_Tabela_parametro(topoLocal, escopo_atual,tipo, PARAMETRO, passagemLocal, cont_dim);
                }
                
                cont_dim = 0;
            }while(t.cat == SN && t.codigo == VIRGULA);

            Veri_Quant_param_menor(Consulta_Tabela(nomeID.lexema, 0), tamanPara);

            if(!(t.cat == SN && t.codigo == FECHA_PAR)) error("Incialização de prot inválida, falta parenteses");
            t.processado = true;
            t = analex();
        }

        endeDeclVar = 0;
        while ((t.cat == PR) && ((t.codigo == CONST) || (t.codigo == INT) || (t.codigo == REAL) || (t.codigo == CHAR) || (t.codigo == BOOL))) {
            TornarVivo(nomeID);
            Decl_list_var();
        }

        while(!(t.cat == PR && t.codigo == ENDP)){
            Func_CMD();
        }
        t.processado = true;
        Remove_Tabela();
        TornarZumbi(nomeID);
        t = analex();
    }
}

void Func_CMD(){
    int tamanPara = 0, topoLocal;
    char label[10], labelInicio[10], labelFim[10];

    if (t.cat == ID) {
        Func_Atrib();
    } else if(t.cat == PR && t.codigo == DO){
        t.processado = true;
        t = analex();
        if(t.cat != ID) error("Identificador esperado"); //testa id
        t.processado = true;
        // ajeitar esse if que esta incorreto
        if(Consulta_Tabela(t.lexema, 0) == -1 || (tabela_simbolos[Consulta_Tabela(t.lexema, 0)].categoria != PROT && tabela_simbolos[Consulta_Tabela(t.lexema, 0)].categoria != PROCED)) error("PROT/PROC nao declarado anteriormente");
        topoLocal = Consulta_Tabela(t.lexema, 0);
        
        strcpy(label, tabela_simbolos[topoLocal].rotulo);

        t = analex();

        if(!(t.cat == SN && t.codigo == ABRE_PAR)) error("CMD -> DO inválida, falta abre parenteses");
        do{
            t.processado = true;
            t = analex();

            if((t.cat == SN && t.codigo == FECHA_PAR)) break;
            tamanPara++;
            
            Func_Expr();

        }while(t.cat == SN && t.codigo == VIRGULA);

        if(!(t.cat == SN && t.codigo == FECHA_PAR)) error("CMD -> DO inválida, falta fecha parenteses");
        t.processado = true;
        Veri_Quant_param_maior(topoLocal, tamanPara);
        Veri_Quant_param_menor(topoLocal, tamanPara);

        escrevePilha("CALL %s\n", label);
        // escrevePilha("CALL %s\n", tabela_simbolos[topoLocal].rotulo);

        t = analex();
    } else if (t.cat == PR && t.codigo == WHILE){
        t.processado = true;

        strcpy(labelInicio, criarLabel());
        escrevePilha("LABEL %s\n", labelInicio);

        t = analex();
        if (!(t.cat == SN && t.codigo == ABRE_PAR)) error("Abre parentese esperado");
        t.processado = true;
        t = analex();

        Func_Expr();

        if (!(t.cat == SN && t.codigo == FECHA_PAR)) error("CMD -> WHILE inválida, falta abre parenteses");
        t.processado = true;

        strcpy(labelFim, criarLabel());
        escrevePilha("GOFALSE %s\n", labelFim);

        t = analex();
        while(!(t.cat == PR && t.codigo == ENDW)){
            Func_CMD();
        }
        t.processado = true;
        escrevePilha("GOTO %s\nLABEL %s\n", labelInicio, labelFim);
        t =analex();
    } else if (t.cat == PR && t.codigo == VAR){
        t.processado = true;
        t = analex();
        if(t.cat != ID) error("Identificador esperado"); //testa id
        t.processado = true;
        if(Consulta_Tabela(t.lexema, 0) == -1)error("Identificador nao declarado");
        t = analex();
        if(!(t.cat == PR && t.codigo == FROM)) error("CMD -> VAR inválida, falta PR FROM");
        t.processado = true;
        t = analex();
        
        Func_Expr();

        if(!(t.cat == PR && (t.codigo == TO || t.codigo == DT))) error("CMD -> VAR inválida, falta PR (TO||DT)");
        t.processado = true;
        t = analex();

        Func_Expr();

        if(t.cat == PR && t.codigo == BY){
            t.processado = true;
            t = analex();
            if(t.cat == CT_I){
                t.processado = true;
                t = analex();
            } else if (t.cat == ID){
                t.processado = true;
                if(Consulta_Tabela(t.lexema, 0) == -1)error("Identificador nao declarado");
                t = analex();
            } else {
                error ("CMD -> VAR inválida, falta PR BY ou constante inteira ou id");
            }
        }
        while(!(t.cat = PR && t.codigo == ENDV)){
            Func_CMD();
        }
        t.processado = true;
        t = analex();
    } else if (t.cat == PR && t.codigo == IF){
        t.processado = true;
        t = analex();
        if (!(t.cat == SN && t.codigo == ABRE_PAR)) error("Abre parentese esperado");
        t.processado = true;
        t = analex();
        Func_Expr();
        if (!(t.cat == SN && t.codigo == FECHA_PAR)) error("CMD -> WHILE inválida, falta abre parenteses");
        t.processado = true;
        t = analex();

        strcpy(labelInicio, criarLabel());
        escrevePilha("GOFALSE %s\n", labelInicio);

        while(!(t.cat == PR && (t.codigo == ELIF || t.codigo == ELSE || t.codigo == ENDI))){
            Func_CMD();
        }
        
        strcpy(labelFim, criarLabel());
        escrevePilha("GOTO %s\n", labelFim);

        while(t.cat == PR && t.codigo == ELIF){
            escrevePilha("LABEL %s\n", labelInicio);

            t.processado = true;
            t = analex();
            if (!(t.cat == SN && t.codigo == ABRE_PAR)) error("Abre parentese esperado");
            t.processado = true;
            t = analex();

            Func_Expr();
            if (!(t.cat == SN && t.codigo == FECHA_PAR)) error("CMD -> WHILE inválida, falta abre parenteses");
            t.processado = true;
            t = analex();

            strcpy(labelInicio, criarLabel());
            escrevePilha("GOFALSE %s\n", labelInicio);

            while(!(t.cat == PR && (t.codigo == ELIF || t.codigo == ELSE || t.codigo == ENDI))){
                Func_CMD();
            }
            escrevePilha("GOTO %s\n", labelFim);
        }
        if(t.cat == PR && t.codigo == ELSE){
            escrevePilha("LABEL %s\n", labelInicio);
            t.processado = true;
            t = analex();
            while(!(t.cat == PR && t.codigo == ENDI)){
                Func_CMD();
            }
            escrevePilha("LABEL %s\n", labelFim);
        }
        t.processado = true;
        t = analex();
    } else if (t.cat == PR && t.codigo == GETOUT){
        t.processado = true;
        t = analex();
    } else if (t.cat == PR && (t.codigo == GETCHAR || t.codigo == GETINT || t.codigo == GETREAL || t.codigo == GETSTR)){
        int codigoGet;
        codigoGet = t.codigo;
        t.processado = true;
        t = analex();
        if(t.cat != ID) error("CMD -> GETS, Identificador esperado"); //testa id
        t.processado = true;
        if(Consulta_Tabela(t.lexema, -1) == -1)error("Identificador nao declarado");
        escreveGetsCmd(codigoGet, Consulta_Tabela(t.lexema, -1));
        t = analex();
    } else if (t.cat == PR && t.codigo == PUTINT){
        int posicaoPutIn;
        t.processado = true;
        t = analex();
        if(t.cat == ID || t.cat == CT_I){
            t.processado = true;
            if(t.cat == ID && Consulta_Tabela(t.lexema, 0) == -1)error("Identificador nao declarado");

            if(t.cat == ID){
                posicaoPutIn = Consulta_Tabela(t.lexema, -1);
                if(tabela_simbolos[posicaoPutIn].passagem == REFERENCIA) escrevePilha("LOADI %d, %d\n", tabela_simbolos[posicaoPutIn].escopo, tabela_simbolos[posicaoPutIn].endereco);
                else escrevePilha("LOAD %d, %d\n", tabela_simbolos[posicaoPutIn].escopo, tabela_simbolos[posicaoPutIn].endereco);
            } else {
                escrevePilha("PUSH %d\n", t.valInt);
            }
            escrevePilha("PUT_I\n");
            t = analex();
        }else{
            error("CMD -> PUTINT, Identificador esperado ou const inteira");
        }
    } else if (t.cat == PR && t.codigo == PUTREAL){
        int posicaoPutRe;
        t.processado = true;
        t = analex();
        if(t.cat == ID || t.cat == CT_R){
            t.processado = true;
            if(t.cat == ID && Consulta_Tabela(t.lexema, 0) == -1)error("Identificador nao declarado");

            if(t.cat == ID){
                posicaoPutRe = Consulta_Tabela(t.lexema, -1);
                if(tabela_simbolos[posicaoPutRe].passagem == REFERENCIA) escrevePilha("LOADI %d, %d\n", tabela_simbolos[posicaoPutRe].escopo, tabela_simbolos[posicaoPutRe].endereco);
                else escrevePilha("LOAD %d, %d\n", tabela_simbolos[posicaoPutRe].escopo, tabela_simbolos[posicaoPutRe].endereco);
            } else {
                escrevePilha("PUSHF %f\n", t.valFloat);
            }
            escrevePilha("PUT_F\n");

            t = analex();
        }else{
            error("CMD -> PUTREAL, Identificador esperado ou const inteira");
        }
    } else if (t.cat == PR && t.codigo == PUTCHAR){
        int posicaoPutChar;
        t.processado = true;
        t = analex();
        if(t.cat == ID || t.cat == CT_C){
            t.processado = true;
            if(t.cat == ID && Consulta_Tabela(t.lexema, 0) == -1)error("Identificador nao declarado");

            if(t.cat == ID){
                posicaoPutChar = Consulta_Tabela(t.lexema, -1);
                if(tabela_simbolos[posicaoPutChar].passagem == REFERENCIA) escrevePilha("LOADI %d, %d\n", tabela_simbolos[posicaoPutChar].escopo, tabela_simbolos[posicaoPutChar].endereco);
                else escrevePilha("LOAD %d, %d\n", tabela_simbolos[posicaoPutChar].escopo, tabela_simbolos[posicaoPutChar].endereco);
            } else {
                escrevePilha("PUSH %d\n", t.lexema);
            }
            escrevePilha("PUT_C\n");

            t = analex();
        }else{
            error("CMD -> PUTCHAR, Identificador esperado ou const inteira");
        }
    } else if (t.cat == PR && t.codigo == PUTSTR){
        t.processado = true;
        t = analex();
        if(t.cat == ID || t.cat == CT_STR){
            t.processado = true;
            if(t.cat == ID && Consulta_Tabela(t.lexema, 0) == -1)error("Identificador nao declarado");
            t = analex();
        }else{
            error("CMD -> PUTSTR, Identificador esperado ou const string");
        }
    } else {
        error("CMD ou Declaracao de variavel incorreta");
    }
}

void Func_Atrib(){
    TOKEN tokenRecebe;
    int posicao;

    if(t.cat != ID) error("Identificador esperado"); 
    t.processado = true;
    posicao = Consulta_Tabela(t.lexema, 0) == -1;
    if(posicao)error("Identificador nao declarado");
    tokenRecebe = t;
    t = analex();

    if(tabela_simbolos[posicao].array != SIMPLES) escrevePilha("LDDLC 0\n");
    while((t.cat == SN) && (t.codigo == ABRE_COL)){ 
        t.processado = true;
        t = analex();

        Func_Expr();

        if(!(t.cat == SN && t.codigo == FECHA_COL)) error("Fecha colchete esperado");
        t.processado = true;
        t = analex(); 
        
    }
    if(tabela_simbolos[posicao].array != SIMPLES) escrevePilha("ADD \n");

    if (!(t.cat == SN && t.codigo == ATRIBUICAO))error("Sinal de igual esperado");
    t.processado = true;
    t = analex();

    //mechendo nessa parte
    Func_Expr();
    if(Veri_Tipo(tipoVerificado, tabela_simbolos[Consulta_Tabela(tokenRecebe.lexema, -1)].tipo) == -1) error("tipos incompativeis - AQUI 4");

    posicao = Consulta_Tabela(tokenRecebe.lexema, -1);
    escrevePilha("STOR %d, %d\n", tabela_simbolos[posicao].escopo, tabela_simbolos[posicao].endereco);
}

void Func_Expr(){
    int aux, codigo;

    Func_ExprSimples();
    aux = tipoVerificado;

    if (t.cat == SN && (t.codigo == IGUALDADE || t.codigo == DIFERENTE || t.codigo == MENOR || t.codigo == MENOR_IGUAL ||
        t.codigo == MAIOR || t.codigo == MAIOR_IGUAL)){
            t.processado = true;
            codigo = t.codigo;
            t = analex();
            Func_ExprSimples();
            if(Veri_Tipo(tipoVerificado, aux) == -1) error("tipos incompativeis - AQUI 3");

            escreveCodCompara(codigo);
    }
}

void Func_ExprSimples(){
    int aux, tipo;
    char label[10];

    if ((t.cat == SN && (t.codigo == ADICAO || t.codigo == SUBTRACAO))){
        t.processado = true;
        t = analex();
    }

    Func_Termo();
    // talvez colocar um escreveOperadores
    aux = tipoVerificado;

    while (t.cat == SN && (t.codigo == ADICAO || t.codigo == SUBTRACAO || t.codigo == COND_ALTERNATIVA)){
        t.processado = true;
        tipo = t.codigo;
        t = analex();

        if(tipo == COND_ALTERNATIVA){
            strcpy(label, criarLabel());
            escrevePilha("COPY \nGOTRUE %s \nPOP \n", label);
        }

        Func_Termo();
        escreveOperacoes(tipo);
        // se a verificacao der como incompativeis
        if(tipo == COND_ALTERNATIVA) escrevePilha("LABEL %s\n", label);
        
        if(Veri_Tipo(tipoVerificado, aux) == -1) error("tipos incompativeis - AQUI 2");
    }   
}

void Func_Termo(){
    int aux, tipo;
    char label[10];
    
    Func_Fator();
    aux = tipoVerificado; //quardo o tipo retornado para comparacao

    while (t.cat == SN && (t.codigo == MULTIPLICACAO || t.codigo == DIVISAO || t.codigo == COND_ADICAO)){
        t.processado = true;
        tipo = t.codigo;
        t = analex();

        if(tipo == COND_ADICAO){
            strcpy(label, criarLabel());
            escrevePilha("COPY \nGOFALSE %s \nPOP \n", label);
        }

        Func_Fator();
        escreveOperacoes(tipo);
        // se a verificacao der como incompativeis
        if(tipo == COND_ADICAO) escrevePilha("LABEL %s\n", label);

        if(Veri_Tipo(tipoVerificado, aux) == -1) error("tipos incompativeis - AQUI -1");
    } 
}

void Func_Fator(){
    char labelInicio[10], labelFim[10];
    int posicao;

    if(t.cat == ID){
        t.processado = true;
        posicao = Consulta_Tabela(t.lexema, -1);
        if(posicao == -1)error("Identificador nao declarado");
        
        tipoVerificado = tabela_simbolos[Consulta_Tabela(t.lexema, -1)].tipo;
        
        escrevePilha("LOAD %d, %d\n", tabela_simbolos[posicao].escopo, tabela_simbolos[posicao].endereco);
        t = analex();

        while((t.cat == SN) && (t.codigo == ABRE_COL)){
            t.processado = true;
            t = analex();

            Func_Expr();

            if(!(t.cat == SN && t.codigo == FECHA_COL)) error("Fecha colchete esperado");
            t.processado = true;
            t = analex(); 
        }

        if(tabela_simbolos[posicao].array != SIMPLES) escrevePilha("ADD \nLDSTK 1\n");
    } else if (t.cat == CT_I || t.cat == CT_C || t.cat == CT_R){
        t.processado = true;
        escreveConst(t);

        tipoVerificado = t.cat; // atualiza para o teste de tipo

        t = analex();
    } else if (t.cat == SN && t.codigo == ABRE_PAR){
        t.processado = true;
        t = analex();
        Func_Expr();
        if (!(t.cat == SN && t.codigo == FECHA_PAR)) error("Fecha parentese esperado");
        t.processado = true;
        t = analex();
    } else if (t.cat == SN && t.codigo == COND_NEGACAO){
        t.processado = true;
        t = analex();
        Func_Fator();
        strcpy(labelInicio, criarLabel());
        strcpy(labelFim, criarLabel());
        escrevePilha("GOTRUE %s\nPUSH 1\nLABEL %s\nPUSH 0\nLABEL%s\n", labelInicio,labelFim,labelInicio,labelFim);
    } else {
        error("Estrutura do FATOR incorreta");
    }
}