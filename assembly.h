#include "data.h"

void print_program_info(char *name);
void print_global_vars(symbol_table_t *scope);

reg_t* create_register(int reg, int line);
reg_t* get_register(int reg);
void print_reg_name(asm_registers_t r);
int check_valid_reg(int op, int arg, int reg);
void register_allocation(operation_t *code);

void spil(reg_t* reg);
char* get_asm_reg(int reg);

void translate_loadI(operation_t* code);
void translate_func_decl(operation_t* code);
void translate_iloc(operation_t* code);

extern void generate_asm(operation_t* code);