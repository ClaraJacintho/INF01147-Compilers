void print_symbol(symbol_t* s);
void print_table(symbol_table_t *t);
void print_stack();

symbol_table_t* create_symbol_table();
void enter_scope();
void leave_scope();
int type_size(type_t t);
symbol_t create_symbol(char* name, lex_val_t* lex_val, type_t type, kind_t kind, int count);
symbol_table_item_t* insert_symbol(char* name, lex_val_t* lex_val, type_t type, kind_t kind);
symbol_table_item_t* create_identifier(lex_val_t *lv, kind_t k, int count);
symbol_table_item_t* creates_id_list(symbol_table_item_t* a, symbol_table_item_t* b);
void insert_item_in_scope(symbol_table_item_t *i);
void insert_id(symbol_table_item_t *first, type_t t);