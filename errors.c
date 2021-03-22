
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "data.h"
#include "errors.h"


void throw_error(int error, symbol_t *a, symbol_t *b){
    va_list arguments;
    switch (error)
    {
    case ERR_DECLARED:
    
        printf("Error on line %i! Variable %s was already declared. Previous declaration was on line %i\n",
                a->declaration_line, a->key, b->declaration_line);
        break;
    
    default:
        break;
    }
    va_end(arguments);

}

