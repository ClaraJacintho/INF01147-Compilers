#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "data.h"

void print_program_info(char *name){
    printf("\t.file\t\"%s\"\n", name);
    printf("\t.text\n");
}

void print_global_vars(symbol_table_t *scope){
    symbol_table_item_t *s = scope->top;
    while(s){
        if(s->item->kind == K_ID){
            printf("\t.comm\t%s,4\n", s->item->key);
        } else if(s->item->kind == K_FUNC){
            printf("\t.globl\t%s\n", s->item->key);
            printf("\t.type\t%s, @function\n", s->item->key);
        }
        s = s->next;
    }
}

interval_t* get_interval(interval_t* root, int reg){
    interval_t* aux = root;
    while(aux){
        if(aux->reg == reg){
            return aux;
        }
        aux = aux->next;
    }
    return NULL;
}

interval_t* create_interval(int reg, int line){
    interval_t* interval = (interval_t *)malloc(sizeof(interval_t));
    interval->reg = reg;
    interval->start = line;
    interval->end = line;
    interval->count = 1;
    interval->next = NULL;
    return interval;
}

int check_valid_reg(int op, int arg, int reg){
    switch(op){
        case JUMPI: return -1;
        case LOADI: return arg == 0 ? -1 : reg;
        case RSUBI:
        case LOADAI:
        case ADDI: return arg == 1 ? -1 : reg;
        case STOREAI: return arg == 2 ? -1 : reg;
        case CBR: return arg == 0 ? reg : -1;
        default: return reg;
    }
}

// calculate live intervals??
void calculate_live_intervals(operation_t *code){
//     vector<interval_t> regs;
    interval_t* root = NULL;
    interval_t* bottom = NULL;
    int op_count = 0;
    while(code){
        // the consequences of my bad decisions from E5
        int r[3] = {0, 0, 0};
        r[0] = check_valid_reg(code->op_code, 0, code->arg0);
        r[1] = check_valid_reg(code->op_code, 1, code->arg1);
        r[2] = check_valid_reg(code->op_code, 2, code->arg2);
        for(int i =0; i < 3; i++){
            if(r[i] > 0){
                if(root){
                    interval_t* interval = get_interval(root, r[i]);
                    if(interval != NULL){
                        interval->count++;
                        interval->end = op_count;
                    } else {
                        bottom->next = create_interval(r[i], op_count);
                        bottom = bottom->next;
                    }
                } else {
                root = create_interval(r[i], op_count);
                root->next = bottom;
                bottom = root;
                }
            }
         }
         op_count++;
         code = code->next;
    }

    while(root){
        printf("r%i: start: %i end:%i count: %i\n", root->reg, root->start, root->end, root->count);
        root = root->next;
    }

}

extern void generate_asm(operation_t* code){
   calculate_live_intervals(code);
}