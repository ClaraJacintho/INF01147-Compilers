#include <stdlib.h>
#include <stdio.h>
#include "data.h"
#include "symbol_table.h"
#include "code_generation.h"

int reg = 0, label = 1;

int gen_reg(){
    return reg++;
}

char* get_reg(int reg){
    switch(reg){
        case RFP: return "fp";
        case RBSS: return "bss";
        case RSP: return "sp";
        case RPC: return "pc";
        case -1: return "AAAAAAAAAA";
        default:{
            char *key = calloc(20, 1);
            sprintf(key, "\"%d\"", reg);
            return key;
        }
    }
}

int gen_label(){
    return label++;
}

int get_func_label(char* key){
    symbol_t* symbol = find_function(key);
    if(symbol == NULL){
        printf("sjgmdjfngdskjng\n");
    }
    return symbol == NULL ? 0  : symbol->label; // 0 == HALT
}

operation_t* gen_code(iloc_code op, int label, int arg0, int arg1, int arg2, operation_t* next){
    operation_t* code = (operation_t *)malloc(sizeof(operation_t));
    code->op_code = op;
    code->label = label;
    code->arg0 = arg0;
    code->arg1 = arg1;
    code->arg2 = arg2;
    code->next = next;
    return code;
}

operation_t* concat_code(operation_t* a, operation_t* b){
    if(a == NULL){
        return b;
    }

    if(b == NULL){
        return a;
    }

    operation_t* aux;
    aux = a;
    while(aux->next != NULL){
        aux = aux->next;
    }

    aux->next = b;
    return a;
}


operation_t* gen_halt(){
    return gen_code(HALT, 0, NULL_INT, NULL_INT, NULL_INT, NULL);
}

operation_t* init(){
    operation_t* halt = gen_halt();
    operation_t* main = gen_code(JUMPI, NULL_INT, get_func_label("main"), NULL_INT, NULL_INT, halt);
    operation_t* init_rsp = gen_code(LOADI, NULL_INT, 1024, RSP, NULL_INT, main);
    operation_t* init_rfp = gen_code(LOADI, NULL_INT, 1024, RFP, NULL_INT, init_rsp);
    operation_t* init_rbss = gen_code(LOADI, NULL_INT, 500, RBSS, NULL_INT, init_rfp);

    return init_rbss;
}

operation_t* gen_function_declaration(node_t* func){
    int s = get_func_size();
    operation_t* update_rsp = gen_code(ADDI, NULL_INT, RSP, s, RSP, NULL); 
    operation_t* update_rfp = gen_code(I2I, get_func_label(func->lex_val->val.name), RSP, RFP, NULL_INT, update_rsp);
    
    return update_rfp;
}

void print_code(operation_t* code){
    while(code){
        if(code->label != NULL_INT){
            printf("L%d: ", code->label);
        }
        switch (code->op_code)
        {
            case JUMPI:
                printf("jumpI -> L%d", code->arg0);
                break;
            
            case HALT:
                printf("halt");
                break;

            case LOADI:
                printf("loadI %d => r%s", code->arg0, get_reg(code->arg1));
                break;
            
            case I2I:
                printf("i2i r%s => r%s", get_reg(code->arg0),  get_reg(code->arg1));
                break;
            
            case ADDI:
                printf("addI r%s, %d -> r%s", get_reg(code->arg0), code->arg1, get_reg(code->arg2));
                break;

            default:
                break;
        }

        printf("\n");
        code = code->next;
    }


}