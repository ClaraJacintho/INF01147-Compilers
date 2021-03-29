#include "data.h"

char* get_type_name(type_t t);
void throw_declared_error(symbol_t* a, symbol_t* b);
void throw_undeclared_error(int line, char* id);
void throw_error(int error, symbol_t *a, symbol_t *b);
void throw_wrong_type_error(int line, char* key, type_t a, type_t b);
void throw_func_string_error(int line);
void throw_kind_error(int line, char* key, kind_t var_k, kind_t use_k);
void throw_string_vector_error(lex_val_t *lv);
void throw_string_max_error(symbol_t* s, int size);
void throw_input_par_error(int line, type_t t);
void throw_output_par_error(int line, type_t t);
void throw_return_par_error(int line, type_t func_t, type_t ex_t);
void throw_shift_par_error(int line, int value);
void throw_wrong_type_args_error(int line, char* func_name, char* param_name, type_t a, type_t b);
void throw_missing_args_error(int line, char* func_name, int n_args, int received_args);
void throw_excess_args_error(int line, char* func_name, int n_args);
void throw_string_char_to_x_error(int line, char* ident, type_t a, type_t b);