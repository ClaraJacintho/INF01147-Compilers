
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "data.h"
#include "errors.h"

void throw_declared_error(symbol_t* a, symbol_t* b){
     printf("Error on line %i! Variable %s was already declared. Previous declaration was on line %i\n",
    a->declaration_line, a->key, b->declaration_line);
}

void throw_undeclared_error(int line, char* key){
   printf("Error on line %i! Identifier %s has not been declared. First use in this function \n",
                line, key);
}

/*
void throw_error(int error, symbol_t *a, symbol_t *b){
    va_list arguments;
    switch (error)
    {
    case ERR_DECLARED:
        printf("Error on line %i! Variable %s was already declared. Previous declaration was on line %i\n",
                a->declaration_line, a->key, b->declaration_line);
        break;
    
    case ERR_UNDECLARED:
        printf("Error on line %i! Identifier %s has not been declared. First use in this function \n",
                a->declaration_line, a->key);
        break;

    case ERR_VARIABLE:
        printf("Error on line %i! Variable %s is not being used correctly.\n",
                a->declaration_line, a->key);
        break;

    case ERR_VECTOR:
        printf("Error on line %i! Vector %s is not being used correctly\n",
                a->declaration_line, a->key);
        break;

    case ERR_FUNCTION:
        printf("Error on line %i! Function %s is not being used correctly.\n",
                a->declaration_line, a->key);
        break;

    case ERR_WRONG_TYPE:
        printf("Error on line %i! Type of %s is %s and is different from the type of value that has been atributed.\n",
                a->declaration_line, a->key, a->type);
        break;

    case ERR_STRING_TO_X:
        printf("Error on line %i! String %s cannot be implicitly converted to other types.\n",
                a->declaration_line, a->key);
        break;

    case ERR_CHAR_TO_X:
        printf("Error on line %i! Char %s cannot be implicitly converted to other types.\n",
                a->declaration_line, a->key);
        break;
    
    case ERR_STRING_MAX:
        printf("Error on line %i! The string %s is bigger than its declared size of %s.\n",
                a->declaration_line, a->key, a->size);
        break;

    case ERR_STRING_VECTOR:
        printf("Error on line %i! The vector %s cannot be declared as string.\n",
                a->declaration_line, a->key);
        break;    

    case ERR_MISSING_ARGS:
        printf("Error on line %i! Function %s is missing arguments.\n",
                a->declaration_line, a->key);
        break;

    case ERR_EXCESS_ARGS:
        printf("Error on line %i! Function %s has too many arguments.\n",
                a->declaration_line, a->key);
        break;
    
    case ERR_WRONG_TYPE_ARGS:
        printf("Error on line %i! Function %s has arguments with the wrong type.\n",
                a->declaration_line, a->key);
        break;

    case ERR_FUNCTION_STRING:
        printf("Error on line %i! Function %s returns, parameters and arguments cannot be type string .\n",
                a->declaration_line, a->key);
        break;

    case ERR_WRONG_PAR_INPUT:
        printf("Error on line %i! Input commands should only be followed by int or float identifiers.\n Expected %s, but got %s.\n",
                a->declaration_line, a->type, b->type);
        break;

    case ERR_WRONG_PAR_OUTPUT:
        printf("Error on line %i! Output commands should only be followed by int or float identifiers and literals.\n Expected %s, but got %s.\n",
                a->declaration_line, a->type, b->type);
        break;
    
    case ERR_WRONG_PAR_RETURN:
        printf("Error on line %i! Incompatible type for return statement.\n Expected %s, but got %s.\n",
                a->declaration_line, a->type, b->type);
        break;

    case ERR_WRONG_PAR_SHIFT:
        printf("Error on line %i! Shift command should have less than 16 as index\n",
                a->declaration_line);
        break;

    default:
        printf("Invalid error code\n");
        break;
    }
    va_end(arguments);

}*/