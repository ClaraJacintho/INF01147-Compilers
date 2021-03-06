extern  void exporta (void *arvore);
extern void libera (void *arvore);

typedef enum token_t{
    SP_CHAR,
    OP,
    ID,
    LIT,
} token_t;

typedef struct lex_val_t {
    int line;
    token_t type;
    union {
        int   n;
        float f; 
        char  c;  
        char *s;
        int   b; // C sucks
    } val;

} lex_val_t;

typedef struct node_t {


} node_t;
