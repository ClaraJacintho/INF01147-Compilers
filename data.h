#ifndef DATA_H
#define DATA_H

#define MAX_CHILDREN 4

#define TRUE 1
#define FALSE 0

typedef enum token_t{
    SP_CHAR,
    OP,
    ID,
    DECL_ID,
    LIT_INT_T,
    LIT_FLOAT_T,
    LIT_BOOL_T,
    LIT_CHAR_T,
    LIT_STR_T,
} token_t;

typedef union val_s{
        int   n;
        float f; 
        char  c;  
        char *s;
        int   b; // C sucks
        char *name;
} val_t;

typedef struct lex_val_s {
    int line;
    token_t type;
    val_t val;

} lex_val_t;

typedef enum node_type_s {
    FUNC,
    COMMANDS,
    IDENT,
    LIT_INT,
    LIT_FLOAT,
    LIT_BOOL,
    LIT_CHAR,
    LIT_STR,
    IN,
    OUT,
    BREAK,
    CONTINUE,
    SHIFT,
    VECTOR,
    UN_OP,
    BIN_OP,
    FUNC_CALL,
    TERN_OP,
    RETURN,
    INIT,
    ATTR,
    IF,
    FOR,
    WHILE,
    NOT_INIT,
} node_type_t;

typedef enum types_e{
    TYPE_X,
    TYPE_CMD,
    TYPE_INT,    
    TYPE_FLOAT,  
    TYPE_BOOL,   
    TYPE_CHAR,   
    TYPE_STRING, 
} type_t;

typedef struct patch_s{
    int* addr;
    struct patch_s* next;
} patch_t;

typedef enum iloc_e{
    NOP,
    HALT,
    JUMPI,
    LOADI,
    I2I,
    ADDI,
    STOREAI,
    LOADAI,
    ADD,
    SUB,
    MULT,
    DIV,
    CMP_GT,
    CMP_LT,
    CMP_LE,
    CMP_GE,
    CMP_EQ,
    CMP_NE
   
} iloc_code;

typedef struct op_s {
    int label;
    iloc_code op_code;
    int arg0;
    int arg1;
    int arg2;
    struct op_s* next;
} operation_t;


typedef struct node_s{
    lex_val_t *lex_val;
    node_type_t node_type;
    type_t type;

    operation_t* code;
    int reg;
    
    // backpatching for short-circuit
    patch_t* patch_true;
    patch_t* patch_false;

    struct node_s *children[MAX_CHILDREN];
    struct node_s *next;

} node_t;

typedef enum kind_s{
    K_ID, 
    K_VEC,
    K_FUNC,
    K_LIT
} kind_t;

typedef struct symbol_table_item_s symbol_table_item_t;
typedef struct symbol_data{
    char *key;
    int declaration_line;
    int address;
    type_t type;
    kind_t kind;
    int n_args;
    symbol_table_item_t *args;
    int size;
    int count; 
    lex_val_t *data;
    int label;
} symbol_t;

struct symbol_table_item_s {
    struct symbol_table_item_s *next;
    symbol_t *item;
};

typedef struct symbol_table_s {
    int size;
    int current_address;
    int named_scope;
    int global;
    symbol_table_item_t *top;
    symbol_table_item_t *bottom;

} symbol_table_t;

typedef struct stack_item_s{
    symbol_table_t *scope;
    struct stack_item_s *next;
} stack_item_t;

typedef struct scope_tree_node_s{
    symbol_table_t *scope;
    struct scope_tree_node_s *parent;
    
    // Each node has a brother because there might be many nested scopes
    // inside another scope - i don't know how to do a dynamically allocated array in C :(
    struct scope_tree_node_s *brother;

    // points to the first child - the rest must be access through its brothers
    struct scope_tree_node_s *children;
} scope_tree_node_t;

struct var_addr_and_scope{
    int addr;
    int scope_type;
};

#endif