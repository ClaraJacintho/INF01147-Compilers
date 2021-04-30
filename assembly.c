#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "data.h"

int op_count;
reg_t* root;

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

reg_t* get_register(reg_t* root, int reg){
    reg_t* aux = root;
    while(aux){
        if(aux->reg == reg){
            return aux;
        }
        aux = aux->next;
    }
    return NULL;
}

reg_t* create_register(int reg, int line){
    reg_t* interval = (reg_t *)malloc(sizeof(reg_t));
    interval->reg = reg;
    interval->start = line;
    interval->end = line;
    interval->count = 1;
    interval->asm_reg = SPILL;
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

void print_reg_name(asm_registers_t r){
    switch (r)
    {
    case EAX:   printf("EAX \n"); break;  
    case EBX:   printf("EBX \n"); break;  
    case ECX:   printf("ECX \n"); break;  
    case EDX:   printf("EDX \n"); break;  
    case SPILL: printf("SPILLED \n"); break;    
    default:
        break;
    }
}

// calculate live intervals??
void calculate_live_intervals(operation_t *code){
    root = NULL;
    reg_t* bottom = NULL;
    op_count = 0;
    while(code){
        // the consequences of my bad decisions from E5
        int r[3] = {0, 0, 0};
        r[0] = check_valid_reg(code->op_code, 0, code->arg0);
        r[1] = check_valid_reg(code->op_code, 1, code->arg1);
        r[2] = check_valid_reg(code->op_code, 2, code->arg2);
        for(int i =0; i < 3; i++){
            if(r[i] > 0){
                if(root){
                    reg_t* interval = get_register(root, r[i]);
                    if(interval != NULL){
                        interval->count++;
                        interval->end = op_count;
                    } else {
                        bottom->next = create_register(r[i], op_count);
                        bottom = bottom->next;
                    }
                } else {
                root = create_register(r[i], op_count);
                root->next = bottom;
                bottom = root;
                }
            }
         }
         op_count++;
         code = code->next;
    }    

    // 0 == FREE
    // otherwise, counts operations untill it is free
    // if there are no free regs, spill
    int asm_register[4] = {0, 0, 0, 0};
    reg_t* aux = root;
    while(aux){
        for(int i = 0; i < 4; i++){
            if(asm_register[i]-aux->start+1 <= 0){
                aux->asm_reg = i;
                asm_register[i] = aux->end;
                break;
            }
        }
        aux = aux->next;
    }

    aux = root;
    while(aux){
        printf("r%i: start: %i end:%i count: %i ", aux->reg, aux->start, aux->end, aux->count);
        print_reg_name(aux->asm_reg);
        aux = aux->next;
    }

}

extern void generate_asm(operation_t* code){
   calculate_live_intervals(code);
}