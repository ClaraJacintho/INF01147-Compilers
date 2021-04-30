#include "data.h"

void print_program_info(char *name);
void print_global_vars(symbol_table_t *scope);

reg_t* get_register(reg_t* root, int reg);
reg_t* create_register(int reg, int line);

int check_valid_reg(int op, int arg, int reg);

extern void generate_asm(operation_t* code);