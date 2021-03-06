%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
int yylex(void);
int yyerror (char const *s);

extern int get_line_number(void);

extern void *arvore;
%}

%union{
    node_t* node;
    lex_val_t* lex_val; 
}

%define parse.error verbose

%token TK_PR_INT
%token TK_PR_FLOAT
%token TK_PR_BOOL
%token TK_PR_CHAR
%token TK_PR_STRING
%token TK_PR_IF
%token TK_PR_THEN
%token TK_PR_ELSE
%token TK_PR_WHILE
%token TK_PR_DO
%token TK_PR_INPUT
%token TK_PR_OUTPUT
%token TK_PR_RETURN
%token TK_PR_CONST
%token TK_PR_STATIC
%token TK_PR_FOREACH
%token TK_PR_FOR
%token TK_PR_SWITCH
%token TK_PR_CASE
%token TK_PR_BREAK
%token TK_PR_CONTINUE
%token TK_PR_CLASS
%token TK_PR_PRIVATE
%token TK_PR_PUBLIC
%token TK_PR_PROTECTED
%token TK_PR_END
%token TK_PR_DEFAULT


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

%token TOKEN_ERRO

%type<lex_val> function_header

%type<node> program
%type<node> function
%type<node> code_block



%%

// ------------------------------------ main blocks ------------------------------------

// initial symbol
program : global_declaration program { $$ = $2;}
		| function program { $$ = insert_command_node($1, $2); arvore = (void*) $$;  }
		| {$$ = NULL;};

// utils
type : TK_PR_INT | TK_PR_FLOAT | TK_PR_BOOL | TK_PR_CHAR | TK_PR_STRING;
literal : TK_LIT_INT | TK_LIT_FLOAT | TK_LIT_FALSE | TK_LIT_TRUE | TK_LIT_CHAR | TK_LIT_STRING;

// global declarations
global_declaration : type global_id_list ';'| TK_PR_STATIC type global_id_list ';';
vector_declaration: TK_IDENTIFICADOR '['TK_LIT_INT']'
global_var_id: TK_IDENTIFICADOR | vector_declaration;
global_id_list : global_var_id ',' global_id_list| global_var_id;


// function declaration
function : function_header code_block { $$ = create_node($1, FUNC); add_child($$, $2);}
function_header : type TK_IDENTIFICADOR '('params_list')' {$$ = $2;}
				| TK_PR_STATIC type TK_IDENTIFICADOR '('params_list')' {$$ = $3;};
params_list : params |
param: type TK_IDENTIFICADOR 
	| TK_PR_CONST type TK_IDENTIFICADOR;

params: param ',' params | param;
code_block : '{' commands '}';

// ------------------------------------ commands ------------------------------------
commands : command commands
	 | ;
command : code_block';'
	| local_var';' 
	| attribution';' 
	| input';'
	| output';'
	| shift';'
	| function_call';'
	| return';' 
	| break';'
	| continue';' 
	| if 
	| for
	| while;

// local vars
var_type : type | TK_PR_STATIC type | TK_PR_CONST type | TK_PR_STATIC TK_PR_CONST type;
init_types : TK_IDENTIFICADOR '[' expression ']' | TK_IDENTIFICADOR;
var_init : TK_IDENTIFICADOR TK_OC_LE init_types | TK_IDENTIFICADOR TK_OC_LE literal;
var : TK_IDENTIFICADOR | var_init;
local_var_list: var ',' local_var_list| var;
local_var : var_type local_var_list;

// attribution
attribution : var_attribution | vector_attribution;
var_attribution : TK_IDENTIFICADOR '=' expression;
vector_attribution : TK_IDENTIFICADOR '[' expression ']' '=' expression;

// io
input: TK_PR_INPUT TK_IDENTIFICADOR;
output: TK_PR_OUTPUT TK_IDENTIFICADOR | TK_PR_OUTPUT literal;

// fuction call
function_call : TK_IDENTIFICADOR '(' args ')'; 
args : expression ',' args | expression |;

// shift
shift_left : TK_IDENTIFICADOR TK_OC_SL TK_LIT_INT | TK_IDENTIFICADOR '[' expression ']' TK_OC_SL TK_LIT_INT;
shift_right : TK_IDENTIFICADOR TK_OC_SR TK_LIT_INT | TK_IDENTIFICADOR '[' expression ']' TK_OC_SR TK_LIT_INT;
shift : shift_left | shift_right;

// PRs
return : TK_PR_RETURN expression;
break : TK_PR_BREAK;
continue : TK_PR_CONTINUE;


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
expression : logic_exp | logic_exp '?' expression ':' expression;

// operands
id_exp_a : TK_IDENTIFICADOR | TK_IDENTIFICADOR'['expression']';
lit_exp_a : TK_LIT_INT | TK_LIT_FLOAT;
operand_exp_a : id_exp_a | lit_exp_a | function_call;
operand_exp_l : TK_LIT_FALSE | TK_LIT_TRUE;
operand: operand_exp_a | operand_exp_l | unary_op term;

// operators
unary_op : '+' | '-' | '!' | '?' | '&'| '*' | '#';
logic_ops : '|' | '&' | TK_OC_OR | TK_OC_AND;
compare_ops : '<' | '>' | TK_OC_LE | TK_OC_GE | TK_OC_EQ | TK_OC_NE;
sum: '+' | '-';
mul: '*' | '/' | '%';
exponent: '^';

// expression definition
logic_exp : logic_exp logic_ops compare_exp| compare_exp;
compare_exp : compare_exp compare_ops sum_exp| sum_exp;
sum_exp : sum_exp sum mul_exp | mul_exp;
mul_exp : mul_exp mul exponent_exp | exponent_exp;
exponent_exp : exponent_exp exponent term | term;
term : '(' expression ')' |  operand ;


%%

int yyerror(char const *s){
	printf("%s on line %d\n", s, get_line_number());
	return 1;
}	
