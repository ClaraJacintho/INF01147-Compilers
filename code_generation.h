#define NULL_INT -1 // cant use null because it's a pointer

#define RFP  -11
#define RBSS -22
#define RSP  -33
#define RPC  -44

int gen_reg();
char* get_reg(int reg);

int gen_label();
int get_func_label(char* key);

iloc_code get_op(lex_val_t* lv);

operation_t* concat_code(operation_t* a, operation_t* b);
operation_t* gen_code(iloc_code op, int label, int arg0, int arg1, int arg2, operation_t* next);

operation_t* init();

operation_t* gen_function_declaration(node_t* func);
operation_t* gen_literal(node_t* lit);
operation_t* gen_load_var(node_t* id);
operation_t* gen_init(node_t* id, node_t* val);
operation_t* gen_attribution(node_t* id, node_t* exp);

void gen_binop(node_t* op);
void gen_unop(node_t* node);
void gen_not(node_t* node);

patch_t* make_patch(int *addr);
void patch(patch_t* p, int label);
patch_t* make_patch_list(patch_t* a, patch_t* b);
void gen_bool_exp(node_t* node);
void gen_and(node_t* node);
void gen_or(node_t* node);

void gen_if(node_t* node);
void gen_ternop(node_t* node);
void gen_for(node_t* node);
void gen_while(node_t* node);

operation_t* gen_args(node_t* params);
operation_t* gen_func_call(node_t* node);
void save_return(node_t* node);
operation_t* gen_return(node_t* node);

void print_code(operation_t* code);