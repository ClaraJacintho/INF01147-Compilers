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
} node_type_t;

typedef struct node_s{
    lex_val_t *lex_val;
    node_type_t type;
    struct node_s *children[MAX_CHILDREN];
    struct node_s *next;

} node_t;

#endif