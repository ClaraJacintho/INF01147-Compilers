void print_type(type_t t);
void print_args(symbol_table_item_t* st, int num_args);
void print_symbol(symbol_t* s);
void print_table(symbol_table_t *t);
void print_stack();

void free_scope(symbol_table_t* scope);
void free_scope_node(scope_tree_node_t *scope);
void free_all_scopes();
void enter_scope();
void leave_scope();

int type_size(type_t t);
type_t get_type(token_t t);
char* get_key(lex_val_t *lv);
int get_size_from_literal(lex_val_t* lv);
int get_size_from_identifier(lex_val_t* lv);

symbol_table_t* create_symbol_table(int named);
symbol_table_item_t *create_symbol_table_item(symbol_t *symbol);
symbol_table_item_t* creates_st_item_list(symbol_table_item_t* a, symbol_table_item_t* b);
symbol_table_item_t* creates_st_item_list_return_b(symbol_table_item_t* a, symbol_table_item_t* b);
symbol_t create_symbol(char* name, lex_val_t* lex_val, type_t type, kind_t kind, int count);

void insert_item_in_scope(symbol_table_item_t *i);
void insert_symbol_table_item_in_scope(symbol_table_item_t *item);
symbol_table_item_t* insert_symbol(char* name, lex_val_t* lex_val, type_t type, kind_t kind);

symbol_t* find_in_current_scope(lex_val_t *lv);
symbol_t* find_symbol(lex_val_t *lv);
symbol_t* find_identifier_symbol(lex_val_t *lv, kind_t k);

symbol_table_item_t* create_identifier(lex_val_t *lv, kind_t k, int count, type_t t);
void insert_id(symbol_table_item_t *first, type_t t);

symbol_table_item_t *create_function(lex_val_t *lv, type_t t, symbol_table_item_t *args);
int insert_params(symbol_table_item_t *args);

void insert_literal(lex_val_t* lv);

symbol_t* get_current_function();

symbol_t* find_function(char* key);
int get_func_size();
struct var_addr_and_scope get_var_addr_and_scope(lex_val_t* lv);
