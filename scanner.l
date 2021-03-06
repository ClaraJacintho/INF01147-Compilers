%{
    #include "parser.tab.h"
    #include "ast.h"
    #include <stdio.h>

    int line_number = 1;

    void get_lex(val_t val, token_t toke_type);
%}

num [0-9]+
single_num [0-9]
alphabetic [a-zA-Z_]
alphanumeric [a-zA-Z0-9_]
/*Special state "COMMENT" to ignore multiline comments*/
%x COMMENT

%%

"int"           return TK_PR_INT;
"float"         return TK_PR_FLOAT;
"bool"          return TK_PR_BOOL;
"char"          return TK_PR_CHAR;
"string"        return TK_PR_STRING;
"if"            return TK_PR_IF;
"then"          return TK_PR_THEN;
"else"          return TK_PR_ELSE;
"while"         return TK_PR_WHILE;
"do"            return TK_PR_DO;
"input"         return TK_PR_INPUT;
"output"        return TK_PR_OUTPUT;
"return"        return TK_PR_RETURN;
"const"         return TK_PR_CONST;
"static"        return TK_PR_STATIC;
"foreach"       return TK_PR_FOREACH;
"for"           return TK_PR_FOR;
"switch"        return TK_PR_SWITCH;
"case"          return TK_PR_CASE;
"break"         return TK_PR_BREAK;
"continue"      return TK_PR_CONTINUE;
"class"         return TK_PR_CLASS;
"private"       return TK_PR_PRIVATE;
"public"        return TK_PR_PUBLIC;
"protected"     return TK_PR_PROTECTED;
"end"           return TK_PR_END;
"default"       return TK_PR_DEFAULT;

 [,;:()[\]{}|*/<>=!&%#^.$?+-]   {get_lex(strdup(yytext), SP_CHAR); return yytext[0];}

"<="            {get_lex(strdup(yytext), OP); return TK_OC_LE;}
">="            {get_lex(strdup(yytext), OP); return TK_OC_GE;}
"=="            {get_lex(strdup(yytext), OP); return TK_OC_EQ;}
"!="            {get_lex(strdup(yytext), OP); return TK_OC_NE;}
"&&"            {get_lex(strdup(yytext), OP); return TK_OC_AND;}
"||"            {get_lex(strdup(yytext), OP); return TK_OC_OR;}
">>"            {get_lex(strdup(yytext), OP); return TK_OC_SR;}
"<<"            {get_lex(strdup(yytext), OP); return TK_OC_SL;}

{num}                            {get_lex(atoi(yytext), LIT);        return TK_LIT_INT;}
{num}\.{single_num}+([eE]{num})? {get_lex(atof(yytext), LIT);        return TK_LIT_FLOAT;}
"false"                          {get_lex(0, LIT);                   return TK_LIT_FALSE;}
"true"                           {get_lex(1, LIT);                   return TK_LIT_TRUE;}
\'.\'                            {get_lex(strdup(yytext[1]), LIT);   return TK_LIT_CHAR;}
\".+\"                           {get_lex(strdup(yytext), LIT);      return TK_LIT_STRING;} // TODO remove ""

{num}+{alphabetic}{alphanumeric}+           {printf("Erro id st num %c", yytext[0]);  return TOKEN_ERRO;}       // identifier that starts w/ num
{num}+\.{num}*{alphabetic}{alphanumeric}*   {printf("Erro id start flo %c", yytext[0]); return TOKEN_ERRO;}     // identifier that starts w/ num
{num}\.                                     {printf("Erro no num af opoint %c", yytext[0]); return TOKEN_ERRO;} // float w/o num after point
\.{num}+                                    {printf("Erro wo lead num%c", yytext[0]); return TOKEN_ERRO;}       // float w/o leading num

{alphabetic}{alphanumeric}*  {get_lex(strdup(yytext), ID); return TK_IDENTIFICADOR;}

[ \t]         { };

[\r\n]            { ++line_number; }

"/*"          { BEGIN(COMMENT); }

"//".*        { };

<COMMENT>"*/" { BEGIN(INITIAL); }

<COMMENT>"\n" { ++line_number; }

<COMMENT>(.) ;

<COMMENT><<EOF>>   {BEGIN(INITIAL); return TOKEN_ERRO;} // multiline comment that doesnt end

.                  {printf("Erro %c", yytext[0]); return TOKEN_ERRO;} // catch all other errors

%%

int get_line_number(void) {
    return line_number;
}

void get_lex(val_t val, token_t toke_type){
    yylval.lex_val = (lex_val_t*)malloc(sizeof(lex_val_t));
    memset(yylval.lex_val, 0, sizeof(lex_val_t));
    int line = get_line_number()

    yylval.lex_val->line = line;
    yylval.lex_val->type = toke_type;
    yylval.lex_val->val = val;
}