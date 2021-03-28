
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "data.h"
#include "errors.h"

char* get_type_name(type_t t){
    switch (t){
        case TYPE_INT: return "int";
        case TYPE_FLOAT: return "float";
        case TYPE_BOOL: return "bool";
        case TYPE_CHAR: return "char";
        case TYPE_STRING: return "string";
        case TYPE_X: return "x";
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
   printf("Error on line %i! Identifier %s has not been declared. First use in this function \n",
                line, key);
    exit(ERR_UNDECLARED);
}

void throw_wrong_type_error(int line, char* key, type_t a, type_t b){
    printf("Error on line %i! Type of %s is %s and is different from the type %s of value that is being atributed.\n",
            line, key, get_type_name(a), get_type_name(b));
    exit(ERR_WRONG_TYPE);
}

void throw_func_string_error(int line){
    printf("Error on line %i! Function cannot have parameters or return type string\n", line);
    exit(ERR_FUNCTION_STRING);
}

void throw_kind_error(int line, char* key, kind_t var_k, kind_t use_k){
    switch (var_k)
    {
    case K_ID:
        printf("Error on line %i! Variable %s is being used as  %s\n", line, key, get_kind_name(use_k));
        exit(ERR_VARIABLE);
        break;
    case K_VEC:
        printf("Error on line %i! Vector %s is being used as  %s\n", line, key, get_kind_name(use_k));
        exit(ERR_VECTOR);
        break;
    case K_FUNC:
        printf("Error on line %i! Function %s is being used as  %s\n", line, key, get_kind_name(use_k));
        exit(ERR_FUNCTION);
        break;
    
    default:
        break;
    }
       
}
