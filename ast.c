#include "ast.h"
#include "parser.tab.h"
#include <stdio.h>

lex_val_t get_lit_lex_val(int line, int type, int val){
    lex_val_t *lex = (lex_val_t*)malloc(sizeof(lex_val_t));
    lex->line = line;
    switch (type)
    {
        case TK_LIT_INT:
            printf("I am an int token");
            lex->val.n = atoi(val);
            break;
        case TK_LIT_FLOAT:
            printf("I am a float token");
            lex->val.f = atof(val);
            break;
         case TK_LIT_TRUE:
            printf("I am true");
            lex->val.b = 1;
            break;
        case TK_LIT_FALSE:
            printf("I am false");
            yylval.lex_val->val.b = 0;
            break;
    default:
        printf("Oh no");
        break;
    }
}