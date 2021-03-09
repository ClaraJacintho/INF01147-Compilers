%{
#include "data.h"
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
int yyerror (char const *s);

extern int get_line_number(void);
extern void *arvore;
%}

%union{
	lex_val_t* lex_val; 
    node_t* node;  
}

%define parse.error verbose

%token<node> TK_PR_INT
%token<node> TK_PR_FLOAT
%token<node> TK_PR_BOOL
%token<node> TK_PR_CHAR
%token<node> TK_PR_STRING
%token<node> TK_PR_IF
%token<node> TK_PR_THEN
%token<node> TK_PR_ELSE
%token<node> TK_PR_WHILE
%token<node> TK_PR_DO
%token<node> TK_PR_INPUT
%token<node> TK_PR_OUTPUT
%token<node> TK_PR_RETURN
%token<node> TK_PR_CONST
%token<node> TK_PR_STATIC
%token<node> TK_PR_FOREACH
%token<node> TK_PR_FOR
%token<node> TK_PR_SWITCH
%token<node> TK_PR_CASE
%token<node> TK_PR_BREAK
%token<node> TK_PR_CONTINUE
%token<node> TK_PR_CLASS
%token<node> TK_PR_PRIVATE
%token<node> TK_PR_PUBLIC
%token<node> TK_PR_PROTECTED
%token<node> TK_PR_END
%token<node> TK_PR_DEFAULT
%token<node> TOKEN_ERRO

%token<lex_val> TK_OC_LE
%token<lex_val> TK_OC_GE
%token<lex_val> TK_OC_EQ
%token<lex_val> TK_OC_NE
%token<lex_val> TK_OC_AND
%token<lex_val> TK_OC_OR
%token<lex_val> TK_OC_SL
%token<lex_val> TK_OC_SR
%token<lex_val> TK_LIT_INT
%token<lex_val> TK_LIT_FLOAT
%token<lex_val> TK_LIT_FALSE
%token<lex_val> TK_LIT_TRUE
%token<lex_val> TK_LIT_CHAR
%token<lex_val> TK_LIT_STRING
%token<lex_val> TK_IDENTIFICADOR

%type<lex_val> function_header 
',' ';' ':' '('')' '[' ']' '{''}' '+' 
'-' '|' '*' '/' '<' '>' '=' '!' '&' 
'%' '#' '^' '.' '$' '?'
unary_op
logic_ops
compare_ops
sum
mul
exponent

%type<node> 
logic_exp
compare_exp
sum_exp
mul_exp
exponent_exp
term
expression
args
operand
operand_exp_a
operand_exp_l
vector_index
lit_exp_a
id_exp_a
shift_left
shift_right
program function 
commands
code_block
command
local_var 
attribution 
input
output
shift
function_call
return
break
continue
if 
for
while
literal

%%

// ------------------------------------ main blocks ------------------------------------

// initial symbol
program : global_declaration program { $$ = $2;}
		| function program { $$ = insert_command_node(&$1, $2); arvore = (void*) $$;  }
		| {$$ = NULL;};

// utils
type : TK_PR_INT | TK_PR_FLOAT | TK_PR_BOOL | TK_PR_CHAR | TK_PR_STRING;
literal : TK_LIT_INT 	 {$$ = create_node($1, LIT_INT);}
		| TK_LIT_FLOAT   {$$ = create_node($1, LIT_FLOAT);}
		| TK_LIT_FALSE   {$$ = create_node($1, LIT_BOOL);}
		| TK_LIT_TRUE    {$$ = create_node($1, LIT_BOOL);}
		| TK_LIT_CHAR    {$$ = create_node($1, LIT_CHAR);}
		| TK_LIT_STRING  {$$ = create_node($1, LIT_STR);};

// global declarations
global_declaration : type global_id_list ';'| TK_PR_STATIC type global_id_list ';';
vector_declaration: TK_IDENTIFICADOR '['TK_LIT_INT']'
global_var_id: TK_IDENTIFICADOR | vector_declaration;
global_id_list : global_var_id ',' global_id_list| global_var_id;


// function declaration
function : function_header code_block { $$ = create_node($1, FUNC); add_child(&$$, $2);}
function_header : type TK_IDENTIFICADOR '('params_list')' {$$ = $2;}
				| TK_PR_STATIC type TK_IDENTIFICADOR '('params_list')' {$$ = $3;};
params_list : params |
param: type TK_IDENTIFICADOR 
	| TK_PR_CONST type TK_IDENTIFICADOR;

params: param ',' params | param;
code_block : '{' commands '}'  {$$ = $2;}; 

// ------------------------------------ commands ------------------------------------
commands : command commands {$$ = insert_command_node(&$1, $2);}
	 | {$$ = NULL;};
command : code_block';' {$$ = $1;}
	| local_var';' 		{$$ = $1;}
	| attribution';' 	{$$ = $1;}
	| input';'			{$$ = $1;}
	| output';' 		{$$ = $1;}
	| shift';'			{$$ = $1;}
	| function_call';'	{$$ = $1;}
	| return';' 		{$$ = $1;}
	| break';'			{$$ = $1;}
	| continue';' 		{$$ = $1;}
	| if 				{$$ = $1;}
	| for				{$$ = $1;}
	| while				{$$ = $1;};

// local vars
var_type : type | TK_PR_STATIC type | TK_PR_CONST type | TK_PR_STATIC TK_PR_CONST type;
init_types : TK_IDENTIFICADOR vector_index | TK_IDENTIFICADOR;
var_init : TK_IDENTIFICADOR TK_OC_LE init_types | TK_IDENTIFICADOR TK_OC_LE literal;
var : TK_IDENTIFICADOR | var_init;
local_var_list: var ',' local_var_list| var;
local_var : var_type local_var_list;

// attribution
attribution : var_attribution | vector_attribution;
var_attribution : TK_IDENTIFICADOR '=' expression;
vector_attribution : TK_IDENTIFICADOR vector_index '=' expression;

// io
input: TK_PR_INPUT TK_IDENTIFICADOR   {$$ = create_node(NULL, IN); add_child(&$$, create_node($2, IDENT));};
output: TK_PR_OUTPUT TK_IDENTIFICADOR {$$ = create_node(NULL, OUT); add_child(&$$, create_node($2, IDENT));}
		| TK_PR_OUTPUT literal 		  {$$ = create_node(NULL, OUT); add_child(&$$, $2);};

// fuction call
function_call : TK_IDENTIFICADOR '(' args ')' {$$ = create_node($1, FUNC_CALL); add_child(&$$, $3);}; 

args : expression ',' args {$$ = insert_command_node(&$1, $3);} 
	| expression {$$ = $1;}
	| {$$ = NULL;};

// shift
shift_left : TK_IDENTIFICADOR TK_OC_SL TK_LIT_INT {$$ = create_node($2, SHIFT); add_child(&$$, create_node($1, IDENT)); add_child(&$$, create_node($3, LIT_INT));}; 
			| TK_IDENTIFICADOR vector_index TK_OC_SL TK_LIT_INT;


shift_right : TK_IDENTIFICADOR TK_OC_SR TK_LIT_INT {$$ = create_node($2, SHIFT); add_child(&$$, create_node($1, IDENT)); add_child(&$$, create_node($3, LIT_INT));};
			| TK_IDENTIFICADOR vector_index TK_OC_SR TK_LIT_INT;

shift : shift_left {$$ = $1;}
	| shift_right {$$ = $1;};

// PRs
return : TK_PR_RETURN expression;
break : TK_PR_BREAK {$$ = create_node(NULL, BREAK);};
continue : TK_PR_CONTINUE {$$ = create_node(NULL, CONTINUE);};


// ------------------------------------ expressions ------------------------------------

if : TK_PR_IF '(' expression ')' code_block
   | TK_PR_IF '(' expression ')' code_block TK_PR_ELSE code_block;

for : TK_PR_FOR '(' attribution ':' expression ':' attribution ')' code_block;

while : TK_PR_WHILE '('expression')' TK_PR_DO code_block;

// for priority, follow the example:
// E → E+T | T
// T → T*F | F
// F → (E) | id
// priority low to high: ternary logic compare +- /%* ^ ()

expression : logic_exp {$$ = $1;}
		| logic_exp '?' expression ':' expression {$$ = create_node(NULL, TERN_OP); add_child(&$$, $1); add_child(&$$, $3); add_child(&$$, $5);}; 


vector_index: '['expression']' {$$ = create_node(NULL, VECTOR); add_child(&$$, $2);};

// operands
id_exp_a : TK_IDENTIFICADOR {$$ = create_node($1, IDENT);}
		| TK_IDENTIFICADOR vector_index {$$ = create_node($1, IDENT); add_child(&$$, $2);};

lit_exp_a : TK_LIT_INT {$$ = create_node($1, LIT_INT);}
		| TK_LIT_FLOAT {$$ = create_node($1, LIT_FLOAT);};

operand_exp_a : id_exp_a  {$$ = $1;}
		| lit_exp_a {$$ = $1;}
		| function_call {$$ = $1;};

operand_exp_l : TK_LIT_FALSE {$$ = create_node($1, LIT_BOOL);}
			| TK_LIT_TRUE {$$ = create_node($1, LIT_BOOL);};

operand: operand_exp_a {$$ = $1;}
		| operand_exp_l {$$ = $1;}
		| unary_op term {$$ = create_node($1, UN_OP); add_child(&$$, $2);};

// operators
unary_op : '+' | '-' | '!' | '?' | '&'| '*' | '#';
logic_ops : '|' | '&' | TK_OC_OR | TK_OC_AND;
compare_ops : '<' | '>' | TK_OC_LE | TK_OC_GE | TK_OC_EQ | TK_OC_NE;
sum: '+' | '-';
mul: '*' | '/' | '%';
exponent: '^';

// expression definition
logic_exp : logic_exp logic_ops compare_exp {$$ = create_node($2, BIN_OP); add_child(&$$, $1); add_child(&$$, $3);}
	| compare_exp {$$ = $1;};

compare_exp : compare_exp compare_ops sum_exp {$$ = create_node($2, BIN_OP); add_child(&$$, $1); add_child(&$$, $3);}
	| sum_exp {$$ = $1;};

sum_exp : sum_exp sum mul_exp {$$ = create_node($2, BIN_OP); add_child(&$$, $1); add_child(&$$, $3);}
	| mul_exp {$$ = $1;};

mul_exp : mul_exp mul exponent_exp {$$ = create_node($2, BIN_OP); add_child(&$$, $1); add_child(&$$, $3);}
	| exponent_exp {$$ = $1;};

exponent_exp : exponent_exp exponent term {$$ = create_node($2, BIN_OP); add_child(&$$, $1); add_child(&$$, $3);}
	| term {$$ = $1;};

term : '(' expression ')' {$$ = $2;}
	|  operand {$$ = $1;};


%%

int yyerror(char const *s){
	printf("%s on line %d\n", s, get_line_number());
	return 1;
}	
