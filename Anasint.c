#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Analex.h"
#include "Anasint.h"


/* Variaveis globais */
TOKEN t;
FILE *fd;
int contLinha;

TOKEN analex(){
    do{
        t = Analex(fd);
        Print_Analex(t);
    }while(t.cat == FIM_EXPR);
    return t;
}

void Prog() {
    // escopo_atual = GBL;
    // t = analex();
    while ((t.cat == PR) &&
           ((t.codigo == CONST) || (t.codigo == INT) || (t.codigo == REAL) ||
            (t.codigo == CHAR) || (t.codigo == BOOL))) {
        Decl_list_var();

    }
    while ((t.cat == PR) && ((t.codigo == PROT) || (t.codigo == DEF))) {
        Decl_def_prot();

    }
    if (t.cat != FIM_ARQ) error("Declaração ou definição de procedimento esperado!");
}

void Decl_list_var(){
    bool testa_const;
    int tipo;

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

    Decl_var(tipo, testa_const);
    printf("\ncat %d\n", t.cat);
    while ((t.cat == SN) && (t.codigo == VIRGULA)) {
        t.processado = true;
        t = analex();
        Decl_var(tipo, testa_const);
    }
    // talvez aqui
}

void Decl_var(int tipo, bool testa_const){
    int k, i;
    bool testa_array;
    int cont_dim = 0;
    int tam_dims[MAX_ARRAY_DIM];

    if(t.cat != ID) error("Identificador esperado");

    t.processado = true;
    t = analex();

    testa_array = ((t.cat == SN) && (t.codigo == ABRE_COL));
    if(testa_array){
        while (testa_array){
            cont_dim++;
            t.processado = true;
            t = analex();
            if(t.cat == CT_I){
                tam_dims[cont_dim - 1] = t.valInt;
                t.processado = true;
                t = analex();
            } else if (t.cat == ID){
                t.processado = true;
                t = analex();
            } else {
                error ("Dimensão de array identificador constante inteiro ou constante inteira esperada");
            }
            if((t.cat != SN) && (t.codigo != FECHA_COL)) error ("Fecha colchete esperado");
            t.processado = true;
            t = analex();
            break;
        }
    }
    if(t.cat == SN && t.codigo == ATRIBUICAO){
        if(testa_array){
            t.processado = true;
            t = analex();
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
                    // t.processado = true;
                    // t = analex();
                    break;
                } else {
                    error("Vírgula ou fecha parênteses esperado");
                }
            }
        } else {
            t.processado = true;
            t = analex();
        }
    }

}

void Decl_def_prot(){
    int cont_dim = 0;
    int tam_dims[MAX_ARRAY_DIM];

    if(t.cat == PR && t.codigo == PROT){
        t.processado = true;
        t = analex();
        
        if(t.cat != ID) error("Identificador esperado"); //testa idprot
        t.processado = true;
        t = analex();

        if(!(t.cat == SN && t.codigo == ABRE_PAR)) error("Incialização de prot inválida, falta parenteses");
        
        do{
            t.processado = true;
            t = analex();

            if ((t.cat == SN) && (t.codigo == ENDERECO)) {
                t.processado = true;
                t = analex();
            }

            if (!((t.cat == PR) && ((t.codigo == INT) || (t.codigo == REAL) ||
            (t.codigo == CHAR) || (t.codigo == BOOL)))) error("Tipo esperado.");

            t.processado = true;
            t = analex();

            while((t.cat == SN) && (t.codigo == ABRE_COL)){
                t.processado = true;
                t = analex();

                if(!(t.cat == SN && t.codigo == FECHA_COL)) error("Fecha colchete esperado");
                t.processado = true;
                t = analex(); 
            }
        }while(t.cat == SN && t.codigo == VIRGULA);

        if(!(t.cat == SN && t.codigo == FECHA_PAR)) error("Incialização de prot inválida, falta parenteses");
        t.processado = true;
        t = analex();

    } else {
        t.processado = true;
        t = analex();

        if((t.cat == PR && t.codigo == INIT)){
            t.processado = true;
            t = analex();
        }
        else if(t.cat == ID ){
            t.processado = true;
            t = analex();
            if(!(t.cat == SN && t.codigo == ABRE_PAR)) error("Incialização de prot inválida, falta parenteses");
        
            do{
                t.processado = true;
                t = analex();

                if ((t.cat == SN) && (t.codigo == ENDERECO)) {
                    t.processado = true;
                    t = analex();
                }

                if (!((t.cat == PR) && ((t.codigo == INT) || (t.codigo == REAL) ||
                (t.codigo == CHAR) || (t.codigo == BOOL)))) error("Tipo esperado.");

                t.processado = true;
                t = analex();

                if(t.cat != ID ) error("Identificador esperado"); //testa id
                t.processado = true;
                t = analex();

                while((t.cat == SN) && (t.codigo == ABRE_COL)){
                    cont_dim++;
                    t.processado = true;
                    t = analex();

                    if(t.cat == CT_I){
                        tam_dims[cont_dim - 1] = t.valInt;
                        t.processado = true;
                        t = analex();
                    } else if (t.cat == ID){
                        t.processado = true;
                        t = analex();
                    } else {
                        error ("Dimensão de array identificador constante inteiro ou constante inteira esperada");
                    }

                    if(!(t.cat == SN && t.codigo == FECHA_COL)) error("Fecha colchete esperado");
                    t.processado = true;
                    t = analex(); 
                }

            }while(t.cat == SN && t.codigo == VIRGULA);

            if(!(t.cat == SN && t.codigo == FECHA_PAR)) error("Incialização de prot inválida, falta parenteses");
            t.processado = true;
            t = analex();
        }

        while ((t.cat == PR) && ((t.codigo == CONST) || (t.codigo == INT) || (t.codigo == REAL) || (t.codigo == CHAR) || (t.codigo == BOOL))) {
            Decl_list_var();
        }

        while(!(t.cat == PR && t.codigo == ENDP)){
            Func_CMD();
        }
        t.processado = true;
        t = analex();
    }
}

void Func_CMD(){
    if (t.cat == ID) {
        Func_Atrib();
    } else if(t.cat == PR && t.codigo == DO){
        t.processado = true;
        t = analex();
        if(t.cat != ID) error("Identificador esperado"); //testa id
        t.processado = true;
        t = analex();

        if(!(t.cat == SN && t.codigo == ABRE_PAR)) error("CMD -> DO inválida, falta abre parenteses");
        do{
            t.processado = true;
            t = analex();
            
            Func_Expr();

        }while(t.cat == SN && t.codigo == VIRGULA);

        if(!(t.cat == SN && t.codigo == FECHA_PAR)) error("CMD -> DO inválida, falta fecha parenteses");
        t.processado = true;
        t = analex();
    } else if (t.cat == PR && t.codigo == WHILE){
        t.processado = true;
        t = analex();
        if (!(t.cat == SN && t.codigo == ABRE_PAR)) error("Abre parentese esperado");
        t.processado = true;
        t = analex();

        Func_Expr();
        if (!(t.cat == SN && t.codigo == FECHA_PAR)) error("CMD -> WHILE inválida, falta abre parenteses");
        t.processado = true;
        t = analex();
        while(!(t.cat == PR && t.codigo == ENDW)){
            Func_CMD();
        }
        t.processado = true;
        t =analex();
    } else if (t.cat == PR && t.codigo == VAR){
        t.processado = true;
        t = analex();
        if(t.cat != ID) error("Identificador esperado"); //testa id
        t.processado = true;
        t = analex();
        if(!(t.cat == PR && t.codigo == FROM)) error("CMD -> VAR inválida, falta PR FROM");
        t.processado = true;
        t = analex();
        
        Func_Expr();

        if(!(t.cat == PR && t.codigo == TO || t.codigo == DT)) error("CMD -> VAR inválida, falta PR (TO||DT)");
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

        while(!(t.cat == PR && (t.codigo == ELIF || t.codigo == ELSE || t.codigo == ENDI))){
            Func_CMD();
        }
        while(t.cat == PR && t.codigo == ELIF){
            t.processado = true;
            t = analex();
            if (!(t.cat == SN && t.codigo == ABRE_PAR)) error("Abre parentese esperado");
            t.processado = true;
            t = analex();

            Func_Expr();
            if (!(t.cat == SN && t.codigo == FECHA_PAR)) error("CMD -> WHILE inválida, falta abre parenteses");
            t.processado = true;
            t = analex();

            while(!(t.cat == PR && (t.codigo == ELIF || t.codigo == ELSE || t.codigo == ENDI))){
                Func_CMD();
            }
        }
        if(t.cat == PR && t.codigo == ELSE){
            t.processado = true;
            t = analex();
            while(!(t.cat == PR && t.codigo == ENDI)){
                Func_CMD();
            }
        }
        t.processado = true;
        t = analex();
    } else if (t.cat == PR && t.codigo == GETOUT){
        t.processado = true;
        t = analex();
    } else if (t.cat == PR && (t.codigo == GETCHAR || t.codigo == GETINT || t.codigo == GETREAL || t.codigo == GETSTR)){
        t.processado = true;
        t = analex();
        if(t.cat != ID) error("CMD -> GETS, Identificador esperado"); //testa id
        t.processado = true;
        t = analex();
    } else if (t.cat == PR && t.codigo == PUTINT){
        t.processado = true;
        t = analex();
        if(t.cat == ID || t.cat == CT_I){
            t.processado = true;
            t = analex();
        }else{
            error("CMD -> PUTINT, Identificador esperado ou const inteira");
        }
    } else if (t.cat == PR && t.codigo == PUTREAL){
        t.processado = true;
        t = analex();
        if(t.cat == ID || t.cat == CT_R){
            t.processado = true;
            t = analex();
        }else{
            error("CMD -> PUTREAL, Identificador esperado ou const inteira");
        }
    } else if (t.cat == PR && t.codigo == PUTCHAR){
        t.processado = true;
        t = analex();
        if(t.cat == ID || t.cat == CT_C){
            t.processado = true;
            t = analex();
        }else{
            error("CMD -> PUTCHAR, Identificador esperado ou const inteira");
        }
    } else if (t.cat == PR && t.codigo == PUTSTR){
        t.processado = true;
        t = analex();
        if(t.cat == ID || t.cat == CT_STR){
            t.processado = true;
            t = analex();
        }else{
            error("CMD -> PUTSTR, Identificador esperado ou const string");
        }
    }
}

void Func_Atrib(){
    if(t.cat != ID) error("Identificador esperado"); //testa id
    t.processado = true;
    t = analex();

    while((t.cat == SN) && (t.codigo == ABRE_COL)){
        t.processado = true;
        t = analex();

        Func_Expr();

        if(!(t.cat == SN && t.codigo == FECHA_COL)) error("Fecha colchete esperado");
        t.processado = true;
        t = analex(); 
    }
    if (!(t.cat == SN && t.codigo == ATRIBUICAO)) error("Sinal de igual esperado");
    t.processado = true;
    t = analex();

    Func_Expr();
}

void Func_Expr(){
    Func_ExprSimples();
    // t.processado = true;
    // t =analex();

    if (t.cat == SN && (t.codigo == IGUALDADE || t.codigo == DIFERENTE || t.codigo == MENOR || t.codigo == MENOR_IGUAL ||
        t.codigo == MAIOR || t.codigo == MAIOR_IGUAL)){
            t.processado = true;
            t = analex();
            Func_ExprSimples();
    }
}

void Func_ExprSimples(){
    if ((t.cat == SN && (t.codigo == ADICAO || t.codigo == SUBTRACAO))){
        t.processado = true;
        t = analex();
    }

    Func_Termo();
    // t.processado = true;
    // t =analex();

    while (t.cat == SN && (t.codigo == ADICAO || t.codigo == SUBTRACAO || t.codigo == COND_ALTERNATIVA)){
        t.processado = true;
        t = analex();

        Func_Termo();
        // t.processado = true;
        // t = analex();
    }   
}

void Func_Termo(){
    
    Func_Fator();

    while (t.cat == SN && (t.codigo == MULTIPLICACAO || t.codigo == DIVISAO || t.codigo == COND_ADICAO)){
        t.processado = true;
        t = analex();

        Func_Fator();
    } 
}

void Func_Fator(){
    if(t.cat == ID){
        t.processado = true;
        t = analex();

        while((t.cat == SN) && (t.codigo == ABRE_COL)){
            t.processado = true;
            t = analex();

            Func_Expr();

            if(!(t.cat == SN && t.codigo == FECHA_COL)) error("Fecha colchete esperado");
            t.processado = true;
            t = analex(); 
        }
    } else if (t.cat == CT_I || t.cat == CT_C || t.cat == CT_R){
        t.processado = true;
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
    } else {
        error("Estrutura do FATOR incorreta");
    }
}