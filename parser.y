%{
	#include <stdio.h>
	#include <stdlib.h>
	#include "symbol_table.h"
	#include "data.h"
	#include "ast.h"
	#include "code_generation.h"


	int yylex(void);
	int yyerror (char const *s);

	extern int get_line_number(void);
	extern void *arvore;
	// since local vars can be nodes on the tree, we can't just assign $$ a symbol table item
	// we need to create a var to store all declarations so we can later attribute a type to them
	symbol_table_item_t *local_list_top = NULL;
	symbol_table_item_t *local_list_bottom = NULL;
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

%type<lex_val>
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
function_header 
vector
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
str_and_char
or_exp
and_exp

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
type : TK_PR_INT   {$$ = TYPE_INT; } 
	| TK_PR_FLOAT  {$$ = TYPE_FLOAT;} 
	| TK_PR_BOOL   {$$ = TYPE_BOOL;} 
	| TK_PR_CHAR   {$$ = TYPE_CHAR;} 
	| TK_PR_STRING {$$ = TYPE_STRING;};
 
literal : TK_LIT_INT 	 {$$ = create_node_literal($1, LIT_INT);}
		| TK_LIT_FLOAT   {$$ = create_node_literal($1, LIT_FLOAT);}
		| TK_LIT_FALSE   {$$ = create_node_literal($1, LIT_BOOL);}
		| TK_LIT_TRUE    {$$ = create_node_literal($1, LIT_BOOL);}
		| TK_LIT_CHAR    {$$ = create_node_literal($1, LIT_CHAR);}
		| TK_LIT_STRING  {$$ = create_node_literal($1, LIT_STR);};

// global declarations
global_declaration : type global_id_list ';' {insert_id($2, $1);}
				| TK_PR_STATIC type global_id_list ';'{insert_id($3, $2);};

vector_declaration: TK_IDENTIFICADOR '['TK_LIT_INT']' {$$ = create_identifier($1, K_VEC, $3->val.n, TYPE_X); insert_literal($3);}

global_var_id: TK_IDENTIFICADOR {$$ = create_identifier($1, K_ID, 1, TYPE_X);}
			| vector_declaration {$$ = $1;};
global_id_list : global_var_id ',' global_id_list {$$ = creates_st_item_list($1, $3);}
				| global_var_id {$$ = creates_st_item_list($1, NULL);};


// function declaration
function : function_header '{' {enter_scope(TRUE);} commands '}'{$$ = create_function_declaration($1, $4); leave_scope();}
function_header : type TK_IDENTIFICADOR '('params_list')' {$$ = create_node($2, FUNC); update_node_type($$, $1); create_function($2, $1, $4);}
				| TK_PR_STATIC type TK_IDENTIFICADOR '('params_list')' {$$ = create_node($3, FUNC); update_node_type($$, $2); create_function($3, $2, $5);};
params_list : params {$$ = $1;}
			| {$$ = NULL;};
param: type TK_IDENTIFICADOR {$$ = create_identifier($2, K_ID, 1, $1);}
	| TK_PR_CONST type TK_IDENTIFICADOR {$$ = create_identifier($3, K_ID, 1, $2);};

params: param ',' params {$$ = creates_st_item_list($1, $3);}
		| param {$$ = creates_st_item_list($1, NULL);};

code_block : '{' {enter_scope(FALSE);} commands '}'{leave_scope(); $$ = $3;}; 

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


vector : TK_IDENTIFICADOR '['expression']' {$$ = create_vector_node(create_node_declared_identifier($1, IDENT, K_VEC), $3);};

// local vars
var_type : type 						{$$ = $1;}
		| TK_PR_STATIC type 			{$$ = $2;}
		| TK_PR_CONST type 				{$$ = $2;}
		| TK_PR_STATIC TK_PR_CONST type {$$ = $3;}; 

var : TK_IDENTIFICADOR TK_OC_LE TK_IDENTIFICADOR {
	local_list_bottom = creates_st_item_list_return_b(local_list_bottom, create_identifier($1, K_ID, get_size_from_identifier($3), TYPE_X));
	if(local_list_top == NULL) local_list_top = local_list_bottom;
	$$ = create_init_node(create_node($1, IDENT), $2, create_node_declared_identifier($3, IDENT, K_ID)); 
}
	| TK_IDENTIFICADOR TK_OC_LE literal {
		local_list_bottom = creates_st_item_list_return_b(local_list_bottom, create_identifier($1, K_ID, get_size_from_literal($3->lex_val), TYPE_X));
		if(local_list_top == NULL) local_list_top = local_list_bottom;
		$$ = create_init_node(create_node($1, IDENT), $2, $3);
};
	| TK_IDENTIFICADOR {
		local_list_bottom = creates_st_item_list_return_b(local_list_bottom, create_identifier($1, K_ID, 1, TYPE_X));
		if(local_list_top == NULL) local_list_top = local_list_bottom;
		$$ = create_node($1, NOT_INIT);  // Creates a node that wont actually be on the tree
}

local_var_list: var ',' local_var_list {$$ = insert_node_next(&$1, $3);}
			| var {$$ = $1;}; 

local_var : var_type local_var_list {$$ = $2; insert_id(local_list_top, $1); update_node_init($$, $1);  local_list_top = NULL; local_list_bottom = NULL;};

// attribution
attribution : var_attribution {$$ = $1;}
			| vector_attribution {$$ = $1;}; 
			
var_attribution : TK_IDENTIFICADOR '=' expression {$$ = create_attrib_node(create_node_declared_identifier($1, IDENT, K_ID), $2 ,$3);}; 
vector_attribution : vector '=' expression {$$ = create_attrib_node($1, $2 ,$3);}; 

// io
input: TK_PR_INPUT TK_IDENTIFICADOR   {$$ = create_input_node(create_node_declared_identifier($2, IDENT, K_ID));};
output: TK_PR_OUTPUT TK_IDENTIFICADOR {$$ = create_output_node(create_node_declared_identifier($2, IDENT, K_ID));}
		| TK_PR_OUTPUT literal 		  {$$ = create_output_node($2);};

// fuction call
function_call : TK_IDENTIFICADOR '(' args ')' {$$ = create_func_call_node($1, $3);}; 

args : expression ',' args {$$ = insert_node_next(&$1, $3);} 
	| expression {$$ = $1;}
	| {$$ = NULL;};

// shift
shift_left : TK_IDENTIFICADOR TK_OC_SL TK_LIT_INT {$$ = create_shift_node(create_node_declared_identifier($1, IDENT, K_ID), $2, create_node_literal($3, LIT_INT));}; 
			| vector TK_OC_SL TK_LIT_INT {$$ = create_shift_node($1, $2, create_node_literal($3, LIT_INT));;};


shift_right : TK_IDENTIFICADOR TK_OC_SR TK_LIT_INT {$$ = create_shift_node(create_node_declared_identifier($1, IDENT, K_ID), $2, create_node_literal($3, LIT_INT));};
			| vector TK_OC_SR TK_LIT_INT {$$ = create_shift_node($1, $2, create_node_literal($3, LIT_INT));};

shift : shift_left {$$ = $1;}
	| shift_right {$$ = $1;};

// PRs
return : TK_PR_RETURN expression {$$ = create_return_node($2); save_return($$);}; 
break : TK_PR_BREAK {$$ = create_cmd_node(BREAK);}; 
continue : TK_PR_CONTINUE {$$ = create_cmd_node(CONTINUE);};


// ------------------------------------ expressions ------------------------------------

if : TK_PR_IF '(' expression ')' code_block {$$ = create_cmd_node(IF); add_child(&$$, $3); add_child(&$$, $5); gen_if($$);}
   | TK_PR_IF '(' expression ')' code_block else {$$ = create_cmd_node(IF); add_child(&$$, $3); add_child(&$$, $5);add_child(&$$, $6); gen_if($$);};

else: TK_PR_ELSE code_block {$$ = $2;}; 

for : TK_PR_FOR '(' attribution ':' expression ':' attribution ')' code_block { $$ = create_cmd_node(FOR); add_child(&$$, $3); add_child(&$$, $5); add_child(&$$, $7);add_child(&$$, $9); gen_for($$);}; 

while : TK_PR_WHILE '('expression')'  TK_PR_DO code_block {$$ = create_cmd_node(WHILE); add_child(&$$, $3); add_child(&$$, $6); gen_while($$);};
// for priority, follow the example:
// E → E+T | T
// T → T*F | F
// F → (E) | id
// priority low to high: ternary logic compare +- /%* ^ ()

expression : or_exp {$$ = $1;}
		| or_exp '?' expression ':' expression {$$ = create_ternop_node( $1, $3, $5); free_lex_val($2); gen_ternop($$);}; 




// operands

// E4D2 - D#3 - strings are now allowed in expressions
str_and_char : TK_LIT_STRING {$$ = create_node_literal($1, LIT_STR);}
			| TK_LIT_CHAR {$$ = create_node_literal($1, LIT_CHAR);};

id_exp_a : TK_IDENTIFICADOR {$$ = create_node_declared_identifier($1, IDENT, K_ID);}
		| vector {$$ = $1;};

lit_exp_a : TK_LIT_INT {$$ = create_node_literal($1, LIT_INT);}
		| TK_LIT_FLOAT {$$ = create_node_literal($1, LIT_FLOAT);};

operand_exp_a : id_exp_a  {$$ = $1;}
		| lit_exp_a {$$ = $1;}
		| function_call {$$ = $1;};

operand_exp_l : TK_LIT_FALSE {$$ = create_node_literal($1, LIT_BOOL);}
			| TK_LIT_TRUE {$$ = create_node_literal($1, LIT_BOOL);};

operand: operand_exp_a {$$ = $1;}
		| operand_exp_l {$$ = $1;}
		| unary_op term {$$ = create_unop_node($1, $2);}
		| str_and_char {$$ = $1;};

// operators
unary_op : '+' | '-' | '!' | '?' | '&'| '*' | '#';
logic_ops : '|' | '&';
compare_ops : '<' | '>' | TK_OC_LE | TK_OC_GE | TK_OC_EQ | TK_OC_NE;
sum: '+' | '-';
mul: '*' | '/' | '%';
exponent: '^';

// expression definition
or_exp: or_exp TK_OC_OR and_exp {$$ = create_binop_node($1, $2, $3); gen_or($$);}
	| and_exp {$$ = $1;};
and_exp: and_exp TK_OC_AND logic_exp {$$ = create_binop_node($1, $2, $3); printf("fufufufufufck\n"); gen_and($$);}
	| logic_exp {$$ = $1;};
logic_exp : logic_exp logic_ops compare_exp {$$ = create_binop_node($1, $2, $3);}
	| compare_exp {$$ = $1;};
compare_exp : compare_exp compare_ops sum_exp {$$ = create_binop_node($1, $2, $3); gen_bool_exp($$);}
	| sum_exp {$$ = $1;};
sum_exp : sum_exp sum mul_exp {$$ = create_binop_node($1, $2, $3); gen_binop($$);}
	| mul_exp {$$ = $1;};
mul_exp : mul_exp mul exponent_exp {$$ = create_binop_node($1, $2, $3); gen_binop($$);}
	| exponent_exp {$$ = $1;};
exponent_exp : exponent_exp exponent term {$$ = create_binop_node($1, $2, $3);}
	| term {$$ = $1;};
term : '(' expression ')' {$$ = $2;}
	|  operand {$$ = $1;};


%%

int yyerror(char const *s){
	printf("%s on line %d\n", s, get_line_number());
	return 1;
}	
