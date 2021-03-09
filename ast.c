#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

lex_val_t *get_lit_lex_val(int line, val_t v){
    lex_val_t *lex = (lex_val_t*)malloc(sizeof(lex_val_t));
    lex->line = line;
    lex->type = LIT;
    lex->val  = v;
    return lex; 
}

lex_val_t *get_lex_val(int line, token_t type, char *name){
    lex_val_t *lex = (lex_val_t*)malloc(sizeof(lex_val_t));
    lex->line = line;
    lex->type = type;
    lex->val.name = name;
    return lex; 
}

void exporta (void *arvore) {
    print_edges(arvore);
    print_node_labels(arvore);
}

void print_edges(void *arvore){
    int i;
    if (arvore != NULL){
        print_edge((node_t *)arvore);
        for(i = 0; i < MAX_CHILDREN; i++){
            print_edges(((node_t *)arvore)->children[i]);
        }
        print_edges(((node_t *)arvore)->next);
    }
}

void print_edge(node_t *node){
    int i;
    if (node != NULL){
        for(i = 0; i < MAX_CHILDREN; i++){
            if(node->children[i] != NULL) {
                printf("%p, %p\n", node, node->children[i]);
            } else {
                break;
            }  
        }

        if(node->next != NULL){
            printf("%p, %p\n", node, node->next);
        }
    }
}

void print_node_labels(void *arvore){
    int i;
    if (arvore != NULL){
        print_label((node_t *)arvore);
        for(i = 0; i < MAX_CHILDREN; i++){
            print_node_labels(((node_t *)arvore)->children[i]);
        }
        print_node_labels(((node_t *)arvore)->next);
    }
}

void print_label(node_t* node){
    if(node != NULL) {
        printf("%p [label=\"", node);
        switch(node->type){
            case IN: printf("input"); break;
            case OUT: printf("output"); break;
            case IDENT:
            case FUNC:
                printf("%s", node->lex_val->val.name);
                break;
            case LIT_BOOL:
            case LIT_INT:
                printf("%i", node->lex_val->val.n);
                break;
            case LIT_FLOAT:
                printf("%f", node->lex_val->val.f);
                break;
            case LIT_CHAR:
                printf("%c", node->lex_val->val.c);
                break;
            case LIT_STR:
                printf("%s", node->lex_val->val.s);
                break;
            case BREAK:
                printf("break");
                break;
            case CONTINUE:
                printf("continue");
                break;
            
            default:
                printf("%i", node->type);
        }
        printf("\"];\n");
    }
    
}

node_t* insert_command_node(node_t** n1, node_t *n2){
    if(*n1 != NULL){
        (*n1)->next = n2;
        return *n1;
    } else {
        return n2;
    }
}

node_t *create_node(lex_val_t *val, node_type_t type){
    node_t *node = (node_t*)malloc(sizeof(node_t));
    memset(node, 0, sizeof(node_t));
    node->lex_val = val;
    node->type = type;
    return node;
}

void add_child(node_t** node, node_t* child){
    int i = 0;
    while((*node)->children[i] != NULL){
        i++;
    }
    (*node)->children[i] = child;
}

