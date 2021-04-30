#include "data.h"

void print_program_info(char *name);
void print_global_vars(symbol_table_t *scope);

int count_lines(operation_t *code);
int check_valid_reg(int op, int arg, int reg);

extern void generate_asm(operation_t* code);