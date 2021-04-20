#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "data.h"
#include "symbol_table.h"
#include "code_generation.h"
#include "ast.h"

// starting number for registers, labels (L0 is halt) 
// and counter for number of instructions in program
int reg = 0, label = 1, gen_instructions = 0;

int gen_reg(){
    return reg++;
}

char* get_reg(int reg){
    switch(reg){
        case RFP: return "fp";
        case RBSS: return "bss";
        case RSP: return "sp";
        case RPC: return "pc";
        case -1: return "OHNO";
        default:{
            char *key = calloc(20, 1);
            sprintf(key, "%d", reg);
            return key;
        }
    }
}

int gen_label(){
    return label++;
}

int get_func_label(char* key){
    symbol_t* symbol = find_function(key);
    return symbol == NULL ? 0  : symbol->label; // 0 == HALT
}

iloc_code get_op(lex_val_t* lv){
    char* op = lv->val.name;
    if(strcmp(op, "+") == 0){
        return ADD;
    } else if(strcmp(op, "-") == 0){
        return SUB;
    } else if(strcmp(op, "/") == 0){
        return DIV;
    } else if(strcmp(op, "*") == 0){
        return MULT;
    } else if(strcmp(op, ">") == 0){
        return CMP_GT;
    } else if(strcmp(op, "<") == 0){
        return CMP_LT;
    } else if(strcmp(op, ">=") == 0){
        return CMP_GE;
    } else if(strcmp(op, "<=") == 0){
        return CMP_LE;
    } else if(strcmp(op, "==") == 0){
        return CMP_EQ;
    } else if(strcmp(op, "!=") == 0){
        return CMP_NE;
    }
    return NOP;
}

operation_t* gen_code(iloc_code op, int label, int arg0, int arg1, int arg2, operation_t* next){
    operation_t* code = (operation_t *)malloc(sizeof(operation_t));
    code->op_code = op;
    code->label = label;
    code->arg0 = arg0;
    code->arg1 = arg1;
    code->arg2 = arg2;
    code->next = next;
    gen_instructions++;
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
    operation_t* init_rbss = gen_code(LOADI, NULL_INT, gen_instructions+1, RBSS, NULL_INT, init_rfp);

    return init_rbss;
}

operation_t* gen_function_declaration(node_t* func){
    int s = get_func_size();
    operation_t* update_rsp = gen_code(ADDI, NULL_INT, RSP, s, RSP, NULL); 
    operation_t* update_rfp = gen_code(I2I, get_func_label(func->lex_val->val.name), RSP, RFP, NULL_INT, update_rsp);
    
    return update_rfp;
}


operation_t* gen_literal(node_t* lit){
    return gen_code(LOADI, NULL_INT, lit->lex_val->val.n, lit->reg, NULL_INT, NULL);
}

operation_t* gen_load_var(node_t* id){
    struct var_addr_and_scope address_scope = get_var_addr_and_scope(id->lex_val);
    int reg = address_scope.scope_type == 1 ? RBSS : RFP;

    return gen_code(LOADAI, NULL_INT, reg, address_scope.addr, id->reg, NULL);
}

operation_t* gen_init(node_t* id, node_t* val){
    struct var_addr_and_scope address_scope = get_var_addr_and_scope(id->lex_val);
    int reg = address_scope.scope_type == 1 ? RBSS : RFP;
    
    // storeAI val_reg => RBSS if global, RFP if local + var shift in stack
    return gen_code(STOREAI, NULL_INT, val->reg, reg, address_scope.addr, NULL);
}

operation_t* gen_attribution(node_t* id, node_t* exp){
    struct var_addr_and_scope address_scope = get_var_addr_and_scope(id->lex_val);

    int reg = address_scope.scope_type == 1 ? RBSS : RFP;
    return gen_code(STOREAI, NULL_INT, exp->reg, reg, address_scope.addr, NULL);
}

void gen_binop(node_t* node){
    node->reg = gen_reg();
    iloc_code operation = get_op(node->lex_val);
    node->code = concat_code( concat_code(node->children[0]->code, node->children[1]->code),
                gen_code(operation, NULL_INT, node->children[0]->reg, node->children[1]->reg, node->reg, NULL));
}

void gen_not(node_t* node){
    node->code = node->children[0]->code;
    node->patch_false = node->children[0]->patch_true;
    node->patch_true = node->children[0]->patch_false;
}

void gen_unop(node_t* node){
    char* op = node->lex_val->val.s;
    if(strcmp(op, "+") == 0){
        node->reg = node->children[0]->reg;
        node->code = node->children[0]->code;
    } else if(strcmp(op, "-") == 0){
        node->code = concat_code( node->children[0]->code, gen_code(RSUBI, NULL_INT, node->children[0]->reg, 0, node->reg, NULL));
    } else  if(strcmp(op, "!") == 0){
       gen_not(node);
    }
}

patch_t* make_patch(int *addr){
    patch_t* patch = (patch_t *)malloc(sizeof(patch_t));
    patch->addr = addr;
    patch->next = NULL;

    return patch;
}

void patch(patch_t* p, int label){
    if(p == NULL){
        return;
    }
    *p->addr = label;
    patch(p->next, label);
}

patch_t* make_patch_list(patch_t* a, patch_t* b){
    if(a == NULL)
        return b;
    else if(b == NULL)
        return a;
    
    patch_t* aux = a;
    while(aux->next != NULL){
        aux = aux->next;
    }
    aux->next = b;
    return a;
}

/**
 * generates the code for a boolean expression, and creates
 * patches in the op node so it can be backpatched
 * @param node node_t
 * @returns a list of generated code
 */
void gen_bool_exp(node_t* node){
    node->reg = gen_reg();
    operation_t* conditional_branch = gen_code(CBR, NULL_INT, node->reg, 0, 0, NULL);
    iloc_code op =  get_op(node->lex_val);
    operation_t* code = gen_code(op, NULL_INT, node->children[0]->reg, node->children[1]->reg, node->reg, conditional_branch);

    node->patch_true = make_patch(&conditional_branch->arg1);
    node->patch_false = make_patch(&conditional_branch->arg2);

    operation_t* inter = concat_code(node->children[0]->code, node->children[1]->code);
    node->code = concat_code(inter, code);
}

void gen_and(node_t* node){
    operation_t* placeholder = gen_code(NOP, gen_label(), NULL_INT, NULL_INT, NULL_INT, NULL);
    patch(node->patch_true, placeholder->label);
    node->patch_true = node->children[1]->patch_true;
    node->patch_false = make_patch_list(node->children[0]->patch_false, node->children[1]->patch_false);
    operation_t* inter = concat_code(node->children[0]->code, placeholder);
    node->code = concat_code(inter, node->children[1]->code);
}

void gen_or(node_t* node){
    operation_t* placeholder = gen_code(NOP, gen_label(), NULL_INT, NULL_INT, NULL_INT, NULL);
    patch(node->patch_false, placeholder->label);
    node->patch_false = node->children[1]->patch_false;
    node->patch_true = make_patch_list(node->children[0]->patch_true, node->children[1]->patch_true);
    operation_t* inter = concat_code(node->children[0]->code, placeholder);
    node->code = concat_code(inter, node->children[1]->code);
}

void gen_if(node_t* node){
    int if_true = gen_label();
    operation_t* if_false = gen_code(NOP, gen_label(), NULL_INT, NULL_INT, NULL_INT, NULL);
    patch(node->children[0]->patch_true, if_true);
    patch(node->children[0]->patch_false, if_false->label);

    node->children[1]->code->label = if_true;
    node->code = concat_code(node->children[0]->code, node->children[1]->code);

    // else
    if(node->children[2] != NULL){
        operation_t* if_else = gen_code(NOP, gen_label(), NULL_INT, NULL_INT, NULL_INT, NULL);
        operation_t* else_jmp = gen_code(JUMPI, NULL_INT, if_else->label, NULL_INT, NULL_INT, if_false);
        operation_t* else_end = gen_code(JUMPI, NULL_INT, if_else->label, NULL_INT, NULL_INT, if_else);
        node->code = concat_code(node->code, concat_code(concat_code(else_jmp, node->children[2]->code), else_end));
    } else {
        node->code = concat_code(node->code, if_false);
    }
}

void gen_ternop(node_t* node){
    node->reg = gen_reg();
    operation_t* if_true = gen_code(NOP, gen_label(), NULL_INT, NULL_INT, NULL_INT, NULL);
    operation_t* if_false = gen_code(NOP, gen_label(), NULL_INT, NULL_INT, NULL_INT, NULL);
    patch(node->children[0]->patch_true, if_true->label);
    patch(node->children[0]->patch_false, if_false->label);

    operation_t* res_v = gen_code(I2I, NULL_INT, node->children[1]->reg, node->reg, NULL_INT, NULL);
    node->code = concat_code(concat_code(concat_code(node->children[0]->code, if_true), node->children[1]->code), res_v); //oh god   

    operation_t* if_else = gen_code(NOP, gen_label(), NULL_INT, NULL_INT, NULL_INT, NULL);
    operation_t* else_jmp = gen_code(JUMPI, NULL_INT, if_else->label, NULL_INT, NULL_INT, if_false);
    operation_t* else_end = gen_code(JUMPI, NULL_INT, if_else->label, NULL_INT, NULL_INT, if_else);
    operation_t* res_f = gen_code(I2I, NULL_INT, node->children[2]->reg, node->reg, NULL_INT, else_end);
    node->code = concat_code(node->code, concat_code(concat_code(else_jmp, node->children[2]->code), res_f));
}

void gen_for(node_t* node){
    int check_cond_lbl = gen_label();
    int condition_true_lbl = gen_label();
    operation_t* condition_false = gen_code(NOP, gen_label(), NULL_INT, NULL_INT, NULL_INT, NULL);

    operation_t* jmp = gen_code(JUMPI, NULL_INT, check_cond_lbl, NULL_INT, NULL_INT, NULL);
    
    patch(node->children[1]->patch_true, condition_true_lbl);
    patch(node->children[1]->patch_false, condition_false->label);

    node->children[1]->code->label = check_cond_lbl;
    node->children[2]->code->label = condition_true_lbl;
    node->code = concat_code(concat_code(node->children[0]->code, node->children[1]->code), node->children[2]->code) ;
    node->code = concat_code(concat_code(concat_code(node->code,node->children[3]->code), jmp), condition_false);

}

void gen_while(node_t* node){
    int check_cond = gen_label();
    int cond_true = gen_label();
    operation_t* condition_false = gen_code(NOP, gen_label(), NULL_INT, NULL_INT, NULL_INT, NULL);
    operation_t* jmp = gen_code(JUMPI, NULL_INT, check_cond, NULL_INT, NULL_INT, NULL);

    patch(node->children[0]->patch_true, cond_true);
    patch(node->children[0]->patch_false, condition_false->label);

    node->children[0]->code->label = check_cond;
    node->children[1]->code->label = cond_true;
    node->code = concat_code(node->children[0]->code, node->children[1]->code);
    node->code = concat_code(concat_code(node->code, jmp), condition_false);

}

operation_t* gen_args(node_t* params){
    operation_t* res = NULL;
    node_t* aux = params;
    int shift = 16; // since we have return addr, return content, RSF and RFP before
    while(aux){
        // save param in function scope
        operation_t* store = gen_code(STOREAI, NULL_INT, aux->reg, RSP, shift, NULL);
        res = concat_code(concat_code(res, aux->code), store);
        shift += 4;
        aux = aux->next;
    }
    return res;
}

operation_t* gen_func_call(node_t* node){
    int reg = gen_reg();
    operation_t* return_val = gen_code(LOADAI, NULL_INT, RSP, 4, node->reg, NULL);
    operation_t* jmp = gen_code(JUMPI, NULL_INT, get_func_label(node->lex_val->val.name), NULL_INT, NULL_INT, return_val);
    operation_t* store_return_pos = gen_code(STOREAI, NULL_INT, reg, RSP, 0, jmp);
    operation_t* get_return_pos = gen_code(ADDI, NULL_INT, RPC, 3, reg , store_return_pos);
    operation_t* args = concat_code(gen_args(node->children[0]), get_return_pos);
    operation_t* store_rfp = gen_code(STOREAI, NULL_INT, RFP, RSP, 8, args);
    operation_t* store_rsp = gen_code(STOREAI, NULL_INT, RSP, RSP, 12, store_rfp);

    return store_rsp;
}

void save_return(node_t* node){
    node->code = concat_code(node->children[0]->code, gen_code(STOREAI, NULL_INT, node->children[0]->reg, RFP, 4, NULL));
}

operation_t* gen_return(node_t* node){
    if(strcmp(node->lex_val->val.name, "main") ==0 ) {
        return gen_code(JUMPI, NULL_INT, 0, NULL_INT, NULL_INT, NULL);
    }
    int reg = gen_reg();
    operation_t* jmp_back = gen_code(JUMP, NULL_INT, reg, NULL_INT, NULL_INT, NULL);
    operation_t* load_rfp = gen_code(LOADAI, NULL_INT, RFP, 8, RFP, jmp_back);
    operation_t* load_rsp = gen_code(LOADAI, NULL_INT, RFP, 12, RSP, load_rfp);
    operation_t* load_return_addr = gen_code(LOADAI, NULL_INT, RFP, 0, reg, load_rsp);

    return load_return_addr;
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
                printf("addI r%s, %d => r%s", get_reg(code->arg0), code->arg1, get_reg(code->arg2));
                break;

            case STOREAI:
                printf("storeAI r%s => r%s, %d", get_reg(code->arg0), get_reg(code->arg1), code->arg2);
                break;
            
            case LOADAI:
                printf("loadAI r%s, %d => r%s", get_reg(code->arg0), code->arg1, get_reg(code->arg2));
                break;
            case  ADD: 
                printf("add r%s, r%s => r%s", get_reg(code->arg0), get_reg(code->arg1), get_reg(code->arg2));
                break;
            case SUB:
                printf("sub r%s, r%s => r%s", get_reg(code->arg0), get_reg(code->arg1), get_reg(code->arg2));
                break;
            case MULT:
                printf("mult r%s, r%s => r%s", get_reg(code->arg0), get_reg(code->arg1), get_reg(code->arg2));
                break;
            case DIV:
                printf("div r%s, r%s => r%s", get_reg(code->arg0), get_reg(code->arg1), get_reg(code->arg2));
                break;
            case CMP_GT:
                printf("cmp_GT r%s, r%s -> r%s", get_reg(code->arg0), get_reg(code->arg1), get_reg(code->arg2));
                break;
            case CMP_LT:
                printf("cmp_LT r%s, r%s -> r%s", get_reg(code->arg0), get_reg(code->arg1), get_reg(code->arg2));
                break;
            case CMP_LE:
                printf("cmp_LE r%s, r%s -> r%s", get_reg(code->arg0), get_reg(code->arg1), get_reg(code->arg2));
                break;
            case CMP_GE:
                printf("cmp_GE r%s, r%s -> r%s", get_reg(code->arg0), get_reg(code->arg1), get_reg(code->arg2));
                break;
            case CMP_EQ:
                printf("cmp_EQ r%s, r%s -> r%s", get_reg(code->arg0), get_reg(code->arg1), get_reg(code->arg2));
                break;
            case CMP_NE:
                printf("cmp_NE r%s, r%s -> r%s", get_reg(code->arg0), get_reg(code->arg1), get_reg(code->arg2));
                break;
            
            case RSUBI:
                printf("rsubI r%s, %d => r%s", get_reg(code->arg0), code->arg1, get_reg(code->arg2));
            break;
            case CBR:
                printf("cbr r%s -> L%d, L%d", get_reg(code->arg0), code->arg1, code->arg2);
            break;
            case JUMP:
                printf("jump -> r%s", get_reg(code->arg0));
            break;
            
            case NOP:
                printf("nop");
                break;
            default: printf("%d", code->op_code);
                break;
        }

        printf("\n");
        code = code->next;
    }


}