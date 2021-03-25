%{
#include "data.h"
#include "ast.h"
#include "symbol_table.h"
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
int yyerror (char const *s);

extern int get_line_number(void);
extern void *arvore;
// since local vars can be nodes on the tree, we can't just assign $$ a symbol table item
// we need to create a var to store all declarations so we can later attribute a type to them
symbol_table_item_t *local_list = NULL;

%}

%union{
	lex_val_t* lex_val; 
    node_t* node;
	type_t type;
	symbol_table_item_t* symbol;  
}

%define parse.error verbose

%token<type> TK_PR_INT
%token<type> TK_PR_FLOAT
%token<type> TK_PR_BOOL
%token<type> TK_PR_CHAR
%token<type> TK_PR_STRING
%token<type> TK_PR_CONST
%token<type> TK_PR_STATIC
%token<node> TK_PR_IF
%token<node> TK_PR_THEN
%token<node> TK_PR_ELSE
%token<node> TK_PR_WHILE
%token<node> TK_PR_DO
%token<node> TK_PR_INPUT
%token<node> TK_PR_OUTPUT
%token<node> TK_PR_RETURN
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
id_with_vector
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
else
for
while
literal 
var
local_var_list   
var_attribution 
vector_attribution

%type<symbol> 
global_var_id
global_id_list
vector_declaration
params_list
params
param

%type<type>
type
var_type

%%

// ------------------------------------ main blocks ------------------------------------

// initial symbol
program : global_declaration program {$$ = $2;}
		| function program { $$ = insert_node_next(&$1, $2); arvore = (void*) $$;  }
		| {$$ = NULL; leave_scope();};

// utils
type : TK_PR_INT   {$$ = TYPE_INT;} 
	| TK_PR_FLOAT  {$$ = TYPE_FLOAT;} 
	| TK_PR_BOOL   {$$ = TYPE_BOOL;} 
	| TK_PR_CHAR   {$$ = TYPE_CHAR;} 
	| TK_PR_STRING {$$ = TYPE_STRING;};
 
literal : TK_LIT_INT 	 {$$ = create_node($1, LIT_INT); insert_literal($1);}
		| TK_LIT_FLOAT   {$$ = create_node($1, LIT_FLOAT); insert_literal($1);}
		| TK_LIT_FALSE   {$$ = create_node($1, LIT_BOOL); insert_literal($1);}
		| TK_LIT_TRUE    {$$ = create_node($1, LIT_BOOL); insert_literal($1);}
		| TK_LIT_CHAR    {$$ = create_node($1, LIT_CHAR); insert_literal($1);}
		| TK_LIT_STRING  {$$ = create_node($1, LIT_STR); insert_literal($1);};

// global declarations
global_declaration : type global_id_list ';' {insert_id($2, $1);}
				| TK_PR_STATIC type global_id_list ';'{insert_id($3, $2);}; // TODO: find out wtf to do w static

vector_declaration: TK_IDENTIFICADOR '['TK_LIT_INT']' {$$ = create_identifier($1, K_VEC, $3->val.n, TYPE_X);}

global_var_id: TK_IDENTIFICADOR {$$ = create_identifier($1, K_ID, 1, TYPE_X);}
			| vector_declaration {$$ = $1;};
global_id_list : global_var_id ',' global_id_list {$$ = creates_st_item_list($1, $3);}
				| global_var_id {$$ = creates_st_item_list($1, NULL);};


// function declaration
function : function_header code_block { $$ = create_node($1, FUNC); add_child(&$$, $2);}
function_header : type TK_IDENTIFICADOR '('params_list')' {$$ = $2; create_function($2, $1, $4);}
				| TK_PR_STATIC type TK_IDENTIFICADOR '('params_list')' {$$ = $3; create_function($3, $2, $5);};
params_list : params {$$ = $1;}
			| {$$ = NULL;};
param: type TK_IDENTIFICADOR {$$ =create_identifier($2, K_ID, 1, $1);}
	| TK_PR_CONST type TK_IDENTIFICADOR {$$ = create_identifier($3, K_ID, 1, $2);};

params: param ',' params {$$ = creates_st_item_list($1, $3);}
		| param {$$ = creates_st_item_list($1, NULL);};

code_block : '{' {enter_scope();} commands '}'{leave_scope(); $$ = $3;}; 

// ------------------------------------ commands ------------------------------------
commands : command commands {$$ = insert_node_next(&$1, $2);}
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


id_with_vector : TK_IDENTIFICADOR vector_index {$$ = create_node($1, IDENT); add_child(&$$, $2);};

// local vars
var_type : type 						{$$ = $1;}
		| TK_PR_STATIC type 			{$$ = $2;}
		| TK_PR_CONST type 				{$$ = $2;}
		| TK_PR_STATIC TK_PR_CONST type {$$ = $3;}; 

var : TK_IDENTIFICADOR TK_OC_LE TK_IDENTIFICADOR {$$ = create_attribution_node(create_node($1, IDENT), $2, create_node($3, IDENT), INIT); 
													local_list = creates_st_item_list(create_identifier($1, K_ID, 1, TYPE_X), local_list);}
	| TK_IDENTIFICADOR TK_OC_LE literal 	{$$ = create_attribution_node(create_node($1, IDENT), $2, $3, INIT);
												local_list = creates_st_item_list(create_identifier($1, K_ID, 1, TYPE_X), local_list);};
	| TK_IDENTIFICADOR {$$ = create_node($1, NOT_INIT);
							local_list = creates_st_item_list(create_identifier($1, K_ID, 1, TYPE_X), local_list);} // Creates a node that wont actually be on the tree

local_var_list: var ',' local_var_list {$$ = insert_node_next(&$1, $3);}
			| var {$$ = $1;}; 

local_var : var_type local_var_list {$$ = $2; insert_id(local_list, $1); local_list = NULL;}; //TODO: add node type to attributions 

// attribution
attribution : var_attribution {$$ = $1;}
			| vector_attribution {$$ = $1;}; 
var_attribution : TK_IDENTIFICADOR '=' expression {$$ = create_node($2, ATTR); add_child(&$$, create_node($1, IDENT)); add_child(&$$, $3);}; 
vector_attribution : id_with_vector '=' expression {$$ = create_node($2, ATTR); add_child(&$$, $1); add_child(&$$, $3);}; 

// io
input: TK_PR_INPUT TK_IDENTIFICADOR   {$$ = create_node(NULL, IN); add_child(&$$, create_node($2, IDENT));};
output: TK_PR_OUTPUT TK_IDENTIFICADOR {$$ = create_node(NULL, OUT); add_child(&$$, create_node($2, IDENT));}
		| TK_PR_OUTPUT literal 		  {$$ = create_node(NULL, OUT); add_child(&$$, $2);};

// fuction call
function_call : TK_IDENTIFICADOR '(' args ')' {$$ = create_node($1, FUNC_CALL); add_child(&$$, $3);}; 

args : expression ',' args {$$ = insert_node_next(&$1, $3);} 
	| expression {$$ = $1;}
	| {$$ = NULL;};

// shift
shift_left : TK_IDENTIFICADOR TK_OC_SL TK_LIT_INT {$$ = create_node($2, SHIFT); add_child(&$$, create_node($1, IDENT)); add_child(&$$, create_node($3, LIT_INT));}; 
			| id_with_vector TK_OC_SL TK_LIT_INT {$$ = create_node($2, SHIFT); add_child(&$$, $1); add_child(&$$, create_node($3, LIT_INT));};


shift_right : TK_IDENTIFICADOR TK_OC_SR TK_LIT_INT {$$ = create_node($2, SHIFT); add_child(&$$, create_node($1, IDENT)); add_child(&$$, create_node($3, LIT_INT));};
			| id_with_vector TK_OC_SR TK_LIT_INT {$$ = create_node($2, SHIFT); add_child(&$$, $1); add_child(&$$, create_node($3, LIT_INT));};

shift : shift_left {$$ = $1;}
	| shift_right {$$ = $1;};

// PRs
return : TK_PR_RETURN expression {$$ = create_node(NULL, RETURN); add_child(&$$, $2);}; 
break : TK_PR_BREAK {$$ = create_node(NULL, BREAK);}; 
continue : TK_PR_CONTINUE {$$ = create_node(NULL, CONTINUE);};


// ------------------------------------ expressions ------------------------------------

if : TK_PR_IF '(' expression ')' code_block {$$ = create_node(NULL, IF); add_child(&$$, $3); add_child(&$$, $5);}
   | TK_PR_IF '(' expression ')' code_block else {$$ = create_node(NULL, IF); add_child(&$$, $3); add_child(&$$, $5);add_child(&$$, $6);};

else: TK_PR_ELSE code_block {$$ = $2;}; 

for : TK_PR_FOR '(' attribution ':' expression ':' attribution ')' code_block { $$ = create_node(NULL, FOR); add_child(&$$, $3); add_child(&$$, $5); add_child(&$$, $7);add_child(&$$, $9);}; 

while : TK_PR_WHILE '('expression')'  TK_PR_DO code_block {$$ = create_node(NULL, WHILE); add_child(&$$, $3); add_child(&$$, $6);};
// for priority, follow the example:
// E → E+T | T
// T → T*F | F
// F → (E) | id
// priority low to high: ternary logic compare +- /%* ^ ()

expression : logic_exp {$$ = $1;}
		| logic_exp '?' expression ':' expression {$$ = create_node(NULL, TERN_OP); add_child(&$$, $1); add_child(&$$, $3); add_child(&$$, $5);free_lex_val($2);}; 


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
