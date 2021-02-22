%{
    #include "parser.tab.h"
    #include <stdio.h>

    int line_number = 1;
%}

signed_num [+-]?[0-9]+
num [0-9]
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

 [,;:()[\]{}|*/<>=!&%#^.$?+-]   return yytext[0];

"<="            return TK_OC_LE;
">="            return TK_OC_GE;
"=="            return TK_OC_EQ;
"!="            return TK_OC_NE;
"&&"            return TK_OC_AND;
"||"            return TK_OC_OR;
">>"            return TK_OC_SL;
"<<"            return TK_OC_SR;

{signed_num}                            return TK_LIT_INT;
{signed_num}\.{num}+([eE]{signed_num})? return TK_LIT_FLOAT;
"false"                                 return TK_LIT_FALSE;
"true"                                  return TK_LIT_TRUE;
\'.\'                                   return TK_LIT_CHAR;
\".+\"                                  return TK_LIT_STRING;

{signed_num}+{alphabetic}{alphanumeric}+          {printf("Erro id st num %c", yytext[0]);  return TOKEN_ERRO;} // identifier that starts w/ num
{signed_num}+\.{num}*{alphabetic}{alphanumeric}*  { printf("Erro id start flo %c", yytext[0]); return TOKEN_ERRO;} // identifier that starts w/ num
{signed_num}\.	                                  { printf("Erro no num af opoint %c", yytext[0]); return TOKEN_ERRO;} // float w/o num after point
\.{num}+                                          { printf("Erro wo lead num%c", yytext[0]); return TOKEN_ERRO;} // float w/o leading num

{alphabetic}{alphanumeric}*  return TK_IDENTIFICADOR;

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

