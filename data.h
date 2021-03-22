#ifndef DATA_H
#define DATA_H

#define MAX_CHILDREN 4

typedef enum token_t{
    SP_CHAR,
    OP,
    ID,
    LIT,
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

typedef struct node_s{
    lex_val_t *lex_val;
    node_type_t type;
    struct node_s *children[MAX_CHILDREN];
    struct node_s *next;

} node_t;

typedef enum types_e{
    TYPE_INT,    
    TYPE_FLOAT,  
    TYPE_BOOL,   
    TYPE_CHAR,   
    TYPE_STRING, 
    TYPE_X,
} type_t;

typedef enum kind_s
{
    K_ID, 
    K_VEC,
    K_FUNC,
} kind_t;

typedef struct symbol_data
{
    char *key;
    int declaration_line;
    type_t type;
    kind_t kind;
    int n_args;
    struct symbol_data *args;
    int size;
    int count; 
    lex_val_t *data;

} symbol_t;

typedef struct symbol_table_item_s {
    struct symbol_table_item_s *next;
    symbol_t *item;
} symbol_table_item_t;

typedef struct symbol_table_s {
    int size;
    symbol_table_item_t *top;
} symbol_table_t;

typedef struct stack_item_s{
    symbol_table_t *scope;
    struct stack_item_s *next;
} stack_item_t;

#endif