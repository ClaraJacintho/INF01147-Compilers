#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "data.h"
#include "error_handling.h"

// inits scope as NULL so that global scope will have ->next == NULL
stack_item_t* current_scope = NULL;

scope_tree_node_t* scope_tree_current = NULL;
scope_tree_node_t* scope_root = NULL;
int n_scopes = 0;

// DEBUG FUNCTIONS
void print_type(type_t t){
    switch (t){
        case TYPE_INT: printf("int \n"); break;
        case TYPE_FLOAT: printf("float \n"); break;
        case TYPE_BOOL: printf("bool \n"); break;
        case TYPE_CHAR: printf("char \n"); break;
        case TYPE_STRING: printf("string \n"); break;
        case TYPE_CMD: printf("command \n"); break;
        case TYPE_X: printf("X \n"); break;
        default:
            break;
    }
}
    
void print_args(symbol_table_item_t* st, int num_args){
    if(st == NULL)
        return;
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
    case K_LIT: printf("literal \n"); break;
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
    printf("    ------------------------ BEGIN TABLE %d --------------------------\n", n_scopes);
    printf("    size: %i\n", t->size);
    symbol_table_item_t *aux = t->top;
    while( aux != NULL){
        print_symbol(aux->item);
        aux = aux->next;
    }
    printf("    ------------------------  END  TABLE %d  ------------------------\n", n_scopes);
}

void print_stack(){
    stack_item_t *aux = current_scope;
    int i = 0;
    while(aux != NULL){
        printf("%i level ===============================================\n", i++);
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

void free_scope(symbol_table_t* scope){
    symbol_table_item_t* st = scope->top;
    while(st != NULL){
        symbol_t* s = st->item;
        printf("FREEING: %s\n", s->key);
        free(s->key);
        if(s->data && (s->data->type == LIT_STR_T || s->data->type == ID)){
            free(s->data->val.s);
        }
        free(s->data);
        if(s->kind == K_FUNC){
            for(int i = 0; i < s->n_args; i++){
                s->args = NULL;
            }
        }
        free(s);
        symbol_table_item_t* aux = st->next;
        free(st);
        st = aux;
    }
    free(scope);

   
    
}

void free_scope_node(scope_tree_node_t *scope){
    if(scope == NULL)
        return;
    free_scope(scope->scope);
    scope_tree_node_t* aux = scope->children;
    while(aux){
        free_scope_node(aux);
        scope_tree_node_t* aux2 = aux;
        aux = aux->brother;
        free(aux2);
    }
    //free(scope->children);
    
}

void free_all_scopes(){
    free_scope_node(scope_root);
    free(scope_root);
    free(scope_tree_current);
    free(current_scope);
}

symbol_t* get_current_function(){
    stack_item_t* aux = current_scope->next;
    if(aux == NULL)
        return NULL;
    
    while(aux != NULL){
        if(aux->scope->bottom->item->kind == K_FUNC){
            return aux->scope->bottom->item;
        }
        aux = aux->next;
    }
    return NULL;

}

int insert_params(symbol_table_item_t *args);

void enter_scope(){
    symbol_table_t* scope = create_symbol_table();
    stack_item_t *stack_item = (stack_item_t *)malloc(sizeof(stack_item_t));
    stack_item->scope = scope;
    stack_item->next = current_scope;
    current_scope = stack_item;
    n_scopes += 1;
    symbol_t* func  = get_current_function();


    if(func != NULL && func->n_args == -1){
        if(func->args != NULL){
            int count = insert_params(func->args);
            func->n_args = count;
        } else {
            func->n_args = 0;
        }
    }

    scope_tree_node_t* scope_node = (scope_tree_node_t *)malloc(sizeof(scope_tree_node_t));
    scope_node->scope = scope;
    scope_node->parent = scope_tree_current;
    scope_node->brother = NULL;
    scope_node->children = NULL;
    if(scope_root == NULL){
        scope_root = scope_node;
    }
    if(scope_tree_current != NULL){
        if(scope_tree_current->children != NULL){
            scope_tree_node_t* aux = scope_tree_current->children;
            while(aux->brother != NULL){
                aux = aux->brother;
            }
            aux->brother = scope_node;
        } else {
            scope_tree_current->children = scope_node;
        }
    }
    scope_tree_current = scope_node;
}

void leave_scope(){
    print_table(current_scope->scope);
    stack_item_t *aux = current_scope;
    current_scope = current_scope->next;
    free(aux);
    scope_tree_current = scope_tree_current->parent;
    n_scopes -= 1;
    
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

char* get_key(lex_val_t *lv){
    char *key;
    switch (lv->type){
        case LIT_INT_T: 
                key = calloc(20, 1);
                sprintf(key, "\"%d\"", lv->val.n);
                break;
                break; 
        case LIT_FLOAT_T:
                key = calloc(20, 1);
                sprintf(key, "\"%f\"", lv->val.f);
                break;
        case LIT_BOOL_T:
                key = calloc(7, 1);
                sprintf(key, "\"%s\"", lv->val.b ? "true" : "false");
                break;

        case LIT_CHAR_T:  
                key = calloc(4, 1);
                sprintf(key, "\'%c\'", lv->val.c);
                break;

        case LIT_STR_T: 
                key = strdup(lv->val.s);;
                break;
        case ID:
        case DECL_ID: 
            key = strdup(lv->val.name);;
            break;
        default:
            printf("%i", lv->type);
            key = strdup(lv->val.name);;
            break;
    }
    return key;

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
    if(type == TYPE_STRING ||kind == K_VEC)
        symbol->size = type_size(type) * count;
    else
        symbol->size = type_size(type);
    symbol->data = lex_val;
    return symbol;
}

symbol_t* find_in_current_scope(lex_val_t *lv){
    char *key = get_key(lv);
    symbol_table_item_t *aux = current_scope->scope->top;
    if(aux != NULL){
        while(aux->next != NULL){
            if(!strcmp(aux->item->key, key)){
                free(key);
                return aux->item;
            }
            aux = aux->next;
        }

        // check if the last one is == to the key
        if(!strcmp(aux->item->key, key)){
            free(key);
            return aux->item;
        }

    }
    free(key);
    return NULL; 
}


void insert_symbol_table_item_in_scope(symbol_table_item_t *item ){
    if(current_scope == NULL)
        enter_scope();
    symbol_t *symbol = item->item;
    symbol_table_item_t *current_top = current_scope->scope->top;
    if(current_top != NULL){
        symbol_t* err = find_in_current_scope(symbol->data);
        if(err != NULL && symbol->kind != K_LIT){
            throw_declared_error(symbol, err);
            return;
        }
        
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

type_t get_type(token_t t){
    switch (t){
        case LIT_INT_T:
            return TYPE_INT;
        case LIT_FLOAT_T:
            return TYPE_FLOAT;
        case LIT_BOOL_T:
            return TYPE_BOOL;
        case LIT_CHAR_T:
            return TYPE_CHAR;
        case LIT_STR_T:
            return TYPE_STRING;
    
        default:
            return TYPE_X;
    }
}

symbol_t* find_symbol(lex_val_t *lv){
    char *key = get_key(lv);
    stack_item_t *s = current_scope;
    while(s != NULL){
        symbol_table_item_t *aux = s->scope->top;
        if(aux != NULL){
            while(aux->next != NULL){
                if(!strcmp(aux->item->key, key)){
                    free(key);
                    return aux->item;
                }
                aux = aux->next;
            }

            // check if the last one is == to the key
            if(!strcmp(aux->item->key, key)){
                free(key);
                return aux->item;
            }

        }
        s = s->next;
    }
    free(key);
    return NULL; 
}

symbol_table_item_t* creates_st_item_list(symbol_table_item_t* a, symbol_table_item_t* b){
    if(b == NULL){
        return a;
    }

    a->next = b;
    return a;
}

symbol_table_item_t* create_identifier(lex_val_t *lv, kind_t k, int count, type_t t, lex_val_t* size){
    char *name = get_key(lv);
    symbol_t *symbol = create_symbol(name, lv, t , k, count);
    return create_symbol_table_item(symbol);
}

void insert_id(symbol_table_item_t *first, type_t t){
    symbol_table_item_t *aux = first;
    while(aux != NULL){
        aux->item->type = t;  
        aux->item->size = type_size(t) * aux->item->count;
        if(aux->item->kind == K_VEC && aux->item->type == TYPE_STRING){
            throw_string_vector_error(aux->item->data);
        }
        insert_symbol(aux->item);
        symbol_table_item_t* aux2 = aux;
        aux = aux->next;
        free(aux2);
    }
}

int insert_params(symbol_table_item_t *args){
    if(current_scope == NULL)
        enter_scope();
    symbol_t* err = NULL;
    int count = 0;
    symbol_table_item_t *aux, *last;
    aux = args;
    last = args;
    symbol_table_t *scope = current_scope->scope;
    while(aux && err == NULL){
        err = find_in_current_scope(aux->item->data);
        if(err != NULL){
            throw_declared_error(aux->item, err);
            return 0;
        }
        if(aux->item->type == TYPE_STRING){
            throw_func_string_error(aux->item->declaration_line);
        }
        count++;
        last = aux;
        aux = aux->next;
    }
    if(err == NULL){
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
    if(t == TYPE_STRING){
        throw_func_string_error(lv->line);
    }
    char *name = get_key(lv);
    symbol_t *symbol = create_symbol(name, lv, t, K_FUNC, 1);
    symbol->args = args;
    symbol->n_args = -1;
    return insert_symbol(symbol);
}

void insert_literal(lex_val_t* lv){
    char* key = get_key(lv);
    int count = 1;
    if(lv->type == LIT_STR_T){
        count = strlen(lv->val.s);
    }
    symbol_t *s = create_symbol(key, lv, get_type(lv->type), K_LIT, count);
    insert_symbol(s);
}

int get_size_from_literal(lex_val_t* lv){
    if(lv->type == LIT_STR_T){
        return strlen(lv->val.s);
    }
    return 1;
}

int get_size_from_identifier(lex_val_t* lv){
    symbol_t* s = find_symbol(lv);
    if(s != NULL){
        return s->count;
    }
    return 0;
}