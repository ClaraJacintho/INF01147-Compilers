
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "data.h"
#include "errors.h"
#include "error_handling.h"

char* get_type_name(type_t t){
    switch (t){
        case TYPE_INT: return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_BOOL: return "bool";
        case TYPE_CHAR: return "char";
        case TYPE_STRING: return "string";
        case TYPE_X: return "x";
        case TYPE_CMD: return "command";
        default: return "";
    }
}

char* get_kind_name(kind_t k){
    switch (k){
        case K_ID: return "variable";
        case K_LIT: return "literal";
        case K_VEC: return "vector";
        case K_FUNC: return "function";
        default: return "";
    }
}

void throw_declared_error(symbol_t* a, symbol_t* b){
    printf("Error on line %i! Identifier %s was already declared. Previous declaration was on line %i\n",
        a->declaration_line, a->key, b->declaration_line);
    exit(ERR_DECLARED);
}

void throw_undeclared_error(int line, char* key){
   printf("Error on line %i! Identifier %s has not been declared.\n",
                line, key);
    exit(ERR_UNDECLARED);
}

void throw_wrong_type_error(int line, char* key, type_t a, type_t b){
    printf("Error on line %i! Type of %s is %s and is different from the type %s of value that is being atributed.\n",
            line, key, get_type_name(a), get_type_name(b));
    exit(ERR_WRONG_TYPE);
}

void throw_func_string_error(int line){
    printf("Error on line %i! Function cannot have parameters, arguments or return type string\n", line);
    exit(ERR_FUNCTION_STRING);
}

void throw_kind_error(int line, char* key, kind_t var_k, kind_t use_k){
    switch (var_k)
    {
    case K_ID:
        printf("Error on line %i! Variable %s is being used as a %s\n", line, key, get_kind_name(use_k));
        exit(ERR_VARIABLE);
        break;
    case K_VEC:
        printf("Error on line %i! Vector %s is being used as a %s\n", line, key, get_kind_name(use_k));
        exit(ERR_VECTOR);
        break;
    case K_FUNC:
        printf("Error on line %i! Function %s is being used as a %s\n", line, key, get_kind_name(use_k));
        exit(ERR_FUNCTION);
        break;
    default:
        break;
    }
       
}

void throw_string_vector_error(lex_val_t *lv){
    printf("Error on line %i! Vector %s cannot be declared with type STRING\n", lv->line, lv->val.s);
    exit(ERR_STRING_VECTOR);
}

void throw_string_max_error(symbol_t* s, int size){
    printf("Error on line %i! Identifier %s has size %d and cannot receive string size %d\n", s->data->line, s->data->val.s, s->count, size);
    exit(ERR_STRING_MAX);
}

void throw_input_par_error(int line, type_t t){
    printf("Error on line %i! Input command only accepts identifiers of type INT and FLOAT, but received type %s instead\n", line, get_type_name(t));
    exit(ERR_WRONG_PAR_INPUT);
}

void throw_output_par_error(int line, type_t t){
    printf("Error on line %i! Output command only accepts identifiers or literals of type INT and FLOAT, but received type %s instead\n", line, get_type_name(t));
    exit(ERR_WRONG_PAR_OUTPUT);
}

void throw_return_par_error(int line, type_t func_t, type_t ex_t){
    printf("Error on line %i! Return type must match function type - Expected %s but got %s\n", line, get_type_name(func_t), get_type_name(ex_t));
    exit(ERR_WRONG_PAR_RETURN);
}

void throw_shift_par_error(int line, int value){
    printf("Error on line %i! Shift value must be less than 16, got %d\n", line, value);
    exit(ERR_WRONG_PAR_SHIFT);
}

void throw_wrong_type_args_error(int line, char* func_name, char* param_name, type_t a, type_t b){
    printf("Error on line %i! Function %s expects parameter %s to have type %s, received %s instead. \n",
            line, func_name, param_name, get_type_name(a), get_type_name(b));
    exit(ERR_WRONG_TYPE_ARGS);
}

void throw_missing_args_error(int line, char* func_name, int n_args, int received_args){
    printf("Error on line %i! Too few arguments for function %s! Expected %i arguments, received %i instead. \n",
            line, func_name, n_args, received_args);
    exit(ERR_MISSING_ARGS);
}

void throw_excess_args_error(int line, char* func_name, int n_args){
    printf("Error on line %i! Too many arguments for function %s! Expected %i argument(s). \n",
            line, func_name, n_args);
    exit(ERR_EXCESS_ARGS);
}

void throw_string_char_to_x_error(int line, char* ident, type_t a, type_t b){
    if(a == TYPE_STRING){
        printf("Error on line %i! Cannot convert %s of type string to %s. \n",
            line, ident, get_type_name(b));
        exit(ERR_STRING_TO_X);
    } else if(a == TYPE_CHAR){
        printf("Error on line %i! Cannot convert %s of type char to %s. \n",
            line, ident, get_type_name(b));
        exit(ERR_CHAR_TO_X);
    }
}
