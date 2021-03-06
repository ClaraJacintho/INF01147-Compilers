#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_CHILDREN 4

extern  void exporta (void *arvore);
extern void libera (void *arvore);

typedef enum token_t{
    SP_CHAR,
    OP,
    ID,
    LIT,
} token_t;

typedef union {
        int   n;
        float f; 
        char  c;  
        char *s;
        int   b; // C sucks
} val_t;

typedef struct lex_val_t {
    int line;
    token_t type;
    val_t val;

} lex_val_t;

typedef enum node_type_t {
    COMMAND,
    FUNC,
    IF
} node_type_t;

typedef struct node_t{
    lex_val_t *lex_val;
    node_type_t type;
    struct node *children[MAX_CHILDREN];
    struct node *next;

} node_t;
