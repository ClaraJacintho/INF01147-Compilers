#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "data.h"
#include "errors.h"

// inits scope as NULL so that global scope will have ->next == NULL
stack_item_t* current_scope = NULL; 

// DEBUG FUNCTIONS
void print_type(type_t t){
    switch (t){
        case TYPE_INT: printf("int \n"); break;
        case TYPE_FLOAT: printf("float \n"); break;
        case TYPE_BOOL: printf("bool \n"); break;
        case TYPE_CHAR: printf("char \n"); break;
        case TYPE_STRING: printf("string \n"); break;
        case TYPE_X: printf("X \n"); break;
        default:
            break;
    }
}
    
void print_args(symbol_table_item_t* st, int num_args){
    symbol_table_item_t *aux = st;
    for(int i = 0; i < num_args; i++){
        printf("            arg_type:");
        print_type(aux->item->type);
        printf("            arg_name: %s\n", aux->item->key); 
        aux = aux->next;
    }
}

void print_symbol(symbol_t* s){
    printf("        ############### key: %s #########################\n", s->key);
    printf("        declaration line: %i\n", s->declaration_line);
    printf("        type: ");
    print_type(s->type);
    printf("        kind: ");
    switch (s->kind){
    case K_ID: printf("identifier \n"); break;
    case K_VEC: printf("vector \n"); break;
    case K_FUNC: printf("func \n"); break;
    default:
        break;
    }
    if(s->n_args > 0){
        printf("        n_args: %i\n", s->n_args);
        print_args(s->args, s->n_args);
    }
    printf("        count: %i\n", s->count); 
    printf("        size: %i\n", s->size); 
    printf("        #################################################\n");
}

void print_table(symbol_table_t *t){
     printf("   ------------------------ BEGIN TABLE ------------------------\n");
    printf("    size: %i\n", t->size);
    symbol_table_item_t *aux =t->top;
    while( aux != NULL){
        print_symbol(aux->item);
        aux = aux->next;
    }
    printf("    ---------------------------  END  TABLE ------------------------\n");
}

void print_stack(){
    stack_item_t *aux = current_scope;
    int i = 0;
    while(aux != NULL){
        printf("%i level ===============================================\n", i);
        print_table(aux->scope);
        aux = aux->next;
    }
}

symbol_table_t* create_symbol_table(){
    symbol_table_t *new_scope = (symbol_table_t *)malloc(sizeof(symbol_table_t));
    new_scope->size = 0;
    new_scope->top = NULL;
    new_scope->bottom = NULL;
    return new_scope;

}

void enter_scope(){
    symbol_table_t* scope = create_symbol_table();
    stack_item_t *stack_item = (stack_item_t *)malloc(sizeof(stack_item_t));
    stack_item->scope = scope;
    stack_item->next = current_scope;
    current_scope = stack_item;
}

void leave_scope(){
    print_stack();
    current_scope = current_scope->next;
    // TODO: free stuff
}

int type_size(type_t t){
    switch (t){
        case TYPE_INT: return 4; 
        case TYPE_FLOAT: return 8;
        case TYPE_BOOL: 
        case TYPE_CHAR:   
        case TYPE_STRING: return 1;
        default:
            return 0;
            break;
    }
}

symbol_t* create_symbol(char* name, lex_val_t* lex_val, type_t type, kind_t kind, int count){
    symbol_t * symbol = (symbol_t*)malloc(sizeof(symbol_t));
    symbol->key = name;
    symbol->declaration_line = lex_val->line;
    symbol->type = type;
    symbol->kind = kind;
    symbol->n_args = 0;
    symbol->args = NULL;
    symbol->count = count;
    symbol->size = type_size(type) * count; // TODO: fix size for strings
    symbol->data = lex_val;
    // TODO: add arguments to functions
    return symbol;
}

int check_if_declared(symbol_t *symbol){
    stack_item_t *s = current_scope;
    while(s != NULL){
    symbol_table_item_t *aux = s->scope->top;
        if(aux != NULL){
            while(aux->next != NULL){
                if(!strcmp(aux->item->key, symbol->key)){
                    throw_error(ERR_DECLARED, symbol, aux->item);
                return 1;
                }
                aux = aux->next;
            }

            // check if the last one is == to the key
            if(!strcmp(aux->item->key, symbol->key)){
                throw_error(ERR_DECLARED, symbol, aux->item);
                return 1;
            }

        }
        s = s->next;
    }
    return 0; 
}


void insert_symbol_table_item_in_scope(symbol_table_item_t *item ){
    if(current_scope == NULL)
        enter_scope();
    symbol_t *symbol = item->item;
    symbol_table_item_t *current_top = current_scope->scope->top;
    if(current_top != NULL){
        int err = check_if_declared(symbol);
        if(err)
            return; //?
        
        current_scope->scope->bottom->next = item;
        current_scope->scope->bottom = item;
      
    } else {
        current_scope->scope->top = item;
        current_scope->scope->bottom = item;
    }
    current_scope->scope->size++;
}
symbol_table_item_t* insert_symbol(symbol_t *symbol){
    symbol_table_item_t *item = (symbol_table_item_t *)malloc(sizeof(symbol_table_item_t));
    item->item = symbol;    
    item->next = NULL;

    insert_symbol_table_item_in_scope(item);

    return item;
}

symbol_table_item_t *create_symbol_table_item(symbol_t *symbol){
    symbol_table_item_t *item = (symbol_table_item_t *)malloc(sizeof(symbol_table_item_t));
    item->item = symbol;
    item->next = NULL;
    return item;
}

// creates identifier with unkonwn type
symbol_table_item_t* create_identifier(lex_val_t *lv, kind_t k, int count, type_t t){
    char *name = (char*)lv->val.s;
    symbol_t *symbol = create_symbol(name, lv, t , k, count);
    return create_symbol_table_item(symbol);
}

symbol_table_item_t* creates_st_item_list(symbol_table_item_t* a, symbol_table_item_t* b){
    if(b == NULL){
        return a;
    }

    a->next = b;
    return a;
}

void insert_id(symbol_table_item_t *first, type_t t){
    symbol_table_item_t *aux = first;
    while(aux != NULL){
        aux->item->type = t; //TODO: check if type compatible w value 
        aux->item->size = type_size(t) * aux->item->count;
        insert_symbol(aux->item);
        // TODO: free this symbol_item
        aux = aux->next;
    }
}


int insert_params(symbol_table_item_t *args){
    if(current_scope == NULL)
        enter_scope();
    int err = 0;
    int count = 0;
    symbol_table_item_t *aux, *last;
    aux = args;
    last = args;
    symbol_table_t *scope = current_scope->scope;
    while(aux && !err){
        err = check_if_declared(aux->item);
        count++;
        last = aux;
        aux = aux->next;
    }
    if(!err){
        printf("DFGDGFSDFGDSFGSDFGSDFG\n");
        // if bottom == NULL than this is the first thing in the scope
        if(scope->bottom == NULL){
            scope->top = args;
        } else {
            scope->bottom->next = args;
        }
        scope->bottom = last;
        scope->size += count;
    }
    return count;
}

symbol_table_item_t *create_function(lex_val_t *lv, type_t t, symbol_table_item_t *args){
    char *name = (char*)lv->val.s;
    symbol_t *symbol = create_symbol(name, lv, t, K_FUNC, 1);
    int count = insert_params(args);
    symbol->args = args;
    symbol->n_args = count;
    return insert_symbol(symbol);
}