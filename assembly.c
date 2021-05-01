#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "data.h"
#include "code_generation.h"
#include "symbol_table.h"
#include "assembly.h"

extern scope_tree_node_t* scope_root;
reg_t* root = NULL;
operation_t* current_op;
int func_count = 0; // why not ¯\_(ツ)_/¯
asm_registers_t arg_regs[6] = {RDI, RSI, EDX, ECX, R8, R9};
int current_arg_index = 0;

void print_program_info(char *name){
    printf("\t.file\t\"%s\"\n", name);
    printf("\t.text\n");
}

void print_global_vars(){
    symbol_table_t *scope = scope_root->scope;
    symbol_table_item_t *s = scope->top;
    while(s){
        if(s->item->kind == K_ID){
            printf("\t.comm\t%s,4\n", s->item->key);
        }
        s = s->next;
    }
}

reg_t* get_register(int reg){
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

// DEBUG
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
void register_allocation(operation_t *code){
    reg_t* bottom = NULL;
    int op_count = 0;
    while(code){
        // the consequences of my bad decisions from E5
        int r[3] = {0, 0, 0};
        r[0] = check_valid_reg(code->op_code, 0, code->arg0);
        r[1] = check_valid_reg(code->op_code, 1, code->arg1);
        r[2] = check_valid_reg(code->op_code, 2, code->arg2);
        for(int i =0; i < 3; i++){
            if(r[i] >= 0){
                if(root){
                    reg_t* interval = get_register(r[i]);
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
    int asm_register[7] = {0,0,0,0,0,0,0};
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

    // // DEBUG
    // aux = root;
    // while(aux){
    //     printf("r%i: start: %i end:%i count: %i ", aux->reg, aux->start, aux->end, aux->count);
    //     print_reg_name(aux->asm_reg);
    //     aux = aux->next;
    // }
}

void spil(reg_t* reg){
    // no thanks
}

char* get_asm_reg(int reg){
    if(reg <  0){
        switch(reg){
            case RFP: return "rbp";
            case RBSS: return (char*)"rip";
            case RSP: return (char*)"rsp";
            case RPC: return (char*)"AAAAA";
        }
    } else {
        reg_t *r = get_register(reg);
        if(r){
            switch (r->asm_reg)
            {
                case EAX: return "eax";
                case EBX: return "ebx";
                case ECX: return "ecx";
                case EDX: return "edx";
                case RDI: return "edi";
                case RSI: return "esi";
                case  R8: return "r8d";
                case  R9: return "r9d";
                case R10: return "r10d";
                case R11: return "r11d";
                case R12: return "r12d";
                case R13: return "r13d";
                case R14: return "r14d";
                case R15: return "r15d";
                case SPILL: spil(r);
                default:
                    break;
            }
        }// else { printf("Frick! %i\n", reg);}
    }
    return "FRICK";
}

char *return_reg_name(int reg){
     switch (reg){
        case EAX: return "eax";
        case EBX: return "ebx";
        case ECX: return "ecx";
        case EDX: return "edx";
        case RDI: return "edi";
        case RSI: return "esi";
        case  R8: return "r8d";
        case  R9: return "r9d";
        case R10: return "r10d";
        case R11: return "r11d";
        case R12: return "r12d";
        case R13: return "r13d";
        case R14: return "r14d";
        case R15: return "r15d";
    }
    return "";
}

void skip_ops(int n){
    for(int i = 0; i < n && current_op != NULL; i++){
        current_op = current_op->next;
    }
}

void translate_loadI(operation_t* code){
    char *s = get_asm_reg(code->arg1);
    printf("\tmovl\t$%d, %%%s\n", code->arg0, s);
}

void translate_func_decl(operation_t* code){
    char *func_name = code->name;
    printf("\t.globl\t%s\n", func_name);
    printf("\t.type\t%s, @function\n", func_name);
    printf("%s:\n", func_name);
    printf(".LFB%i\n", func_count);
    printf("\t.cfi_startproc\n\tendbr64\n\tpushq\t%%rbp\n\t.cfi_def_cfa_offset 16\n\t.cfi_offset 6, -16\n\tmovq\t%%rsp, %%rbp\n");
    skip_ops(2);
}

void translate_return(operation_t* code){
    char *func_name = code->name;
    if(!strcmp(func_name, "main")){
        printf("\tleave\n");
    } else {
        printf("\tpopq\t%%rbp\n");
    }
    printf("\t.cfi_def_cfa 7, 8\n\tret\n\t.cfi_endproc\n");
    printf(".LFE%i\n", func_count++);
    printf("\t.size\t%s, .-%s\n", func_name,func_name);
    skip_ops(4); // trust the process
}

void translate_val_ret(operation_t* code){
    char* r = get_asm_reg(code->next->arg0); // trust the process
    printf("\tmovl\t%s, %%eax\n", r);
    skip_ops(6); // trust the process
}

void translate_storeAI(operation_t* code){

    if (code->arg1 == RBSS){
        printf("\tmovl\t%%%s, %s(%%rip)\n", get_asm_reg(code->arg0), code->name);
    }else{
        printf("\tmovl\t%%%s, -%d(%%%s)\n",get_asm_reg(code->arg0), code->arg2, get_asm_reg(code->arg1));
    }
}

void translate_func_call(operation_t* code){
    skip_ops(2);
    //printf("FIRST NEXT OP %i\n", current_op->op_code);
    if(!(current_op->op_code == ADDI && current_op->arg0 == RPC && current_op->arg1 == 3)){
        while(current_op->op_code != ARG_FIN){
            translate_iloc(current_op);
            current_op = current_op->next;
        }
    }
    printf("\tcall\t%s\n", code->name);
    skip_ops(4);
    //printf("NEXT OP %i\n", current_op->op_code);
}

void translate_arg_declaration(operation_t* code){
    current_op = code->next;
    asm_registers_t arg_regs[6] = {RDI, RSI, EDX, ECX, R8, R9};
    int i = 0;
    while(current_op->op_code != ARG_FIN){
        printf("\tmovl\t%%%s, -%d(rbp)\n", return_reg_name(arg_regs[i]), current_op->arg0);
        i++;
        current_op = current_op->next;
    }
    skip_ops(1);
}

void translate_arg_passage(operation_t* code){
    //printf("qskdkjhlsqdkjflsqkjhflsqkdjhlqkjdfhlswkdjhflskdkjhflqsdfjh\n");
    current_op = code->next;
    if(current_arg_index < 6){
        printf("\tmovl\t%%%s, %%%s\n", get_asm_reg(current_op->arg0), return_reg_name(arg_regs[current_arg_index]));
        current_arg_index++;
    }
}

void reset_current_arg_index(){
    current_arg_index = 0;
}

void translate_loadAI(operation_t* code){
    if(code->arg0 == RBSS){
        printf("\tmovl\t%s(%%rip), %%%s\n", code->name, get_asm_reg(code->arg2));
    }else{
        printf("\tmovl\t-%d(%%%s), %%%s\n", code->arg1, get_asm_reg(code->arg0), get_asm_reg(code->arg2));
    }
}

void translate_binop(char* op, operation_t* code){
    if(!strcmp(op, "idivl")){
        printf("\tmovl\t%%%s, %%eax\n", get_asm_reg(code->arg0));
        printf("\tcdq\n");
        printf("\tidivl\t%%%s", get_asm_reg(code->arg1));
        printf("\tmovl\t%%eax, %%%s\n", get_asm_reg(code->arg2));
    } else {
        printf("\tmovl\t%%%s, %%%s\n", get_asm_reg(code->arg0), get_asm_reg(code->arg2));
        printf("\t%s\t%%%s, %%%s\n", op, get_asm_reg(code->arg1), get_asm_reg(code->arg2));
    }
    
}

void translate_iloc(operation_t* code){
    switch (current_op->op_code)
    {   
        case FUNC_DECL: translate_func_decl(current_op); break;
        case FUNC_RET: translate_return(current_op); break;
        case VAL_RET: translate_val_ret(current_op); break;
        case CALL_FUNC: translate_func_call(current_op); break;
        case ARG_DECL: translate_arg_declaration(current_op); break;
        case ARG_PASS: translate_arg_passage(current_op); break;
        case ARG_FIN: reset_current_arg_index(); break;

        case NOP: printf("\tnop\n"); break;
        case JUMPI: 
            printf("\tjmp\t.L%d\n", current_op->arg0); break;
        case LOADI: translate_loadI(current_op); break;
        case I2I:  printf("\tmovq\t%%%s, %%%s\n", get_asm_reg(current_op->arg0), get_asm_reg(current_op->arg1)); break;
        case ADDI: printf("\taddl\t%s, %i\n", get_asm_reg(current_op->arg0), current_op->arg1); break;
        case STOREAI: translate_storeAI(current_op); break;
        case LOADAI: translate_loadAI(current_op); break;
        case ADD:  translate_binop("addl", current_op); break;
        case SUB:  translate_binop("subl", current_op); break;
        case MULT: translate_binop("imull", current_op); break;
        case DIV:  translate_binop("idivl", current_op); break;
        case CMP_GT:
        case CMP_LT:
        case CMP_LE:
        case CMP_GE:
        case CMP_EQ:
        case CMP_NE:
        case RSUBI:
        case CBR:
        case JUMP:
            /* code */
            break;
        
        default:
            break;
    }
}

void translate_code(operation_t* code){
    current_op = code;
    while(current_op){
        translate_iloc(current_op);
        current_op = current_op != NULL ? current_op->next : NULL;
    }
}

extern void generate_asm(operation_t* code){
    printf("###### start ASM ######\n");
    print_program_info("foo.txt"); // TODO: find out how to get the program name here
    print_global_vars();
    register_allocation(code);
    translate_code(code);
}