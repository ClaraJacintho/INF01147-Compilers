#define NULL_INT -1 // cant use null because it's a pointer

#define RFP  -11
#define RBSS -22
#define RSP  -33
#define RPC  -44

int gen_reg();
char* get_reg(int reg);

int gen_label();
int get_func_label(char* key);

operation_t* concat_code(operation_t* a, operation_t* b);
operation_t* gen_code(iloc_code op, int label, int arg0, int arg1, int arg2, operation_t* next);

operation_t* init();

operation_t* gen_function_declaration(node_t* func);
operation_t* gen_literal(node_t* lit);
operation_t* gen_init(node_t* id, node_t* val);

void print_code(operation_t* code);