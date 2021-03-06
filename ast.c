#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
void exporta (void *arvore) {

}


void print_node_label(void *arvore){
    node_t* tree = (node_t*)arvore;
    if(tree != NULL) {
        printf("%p [label=\"", tree);
        switch(tree->type){
            case FUNC:
                printf("%s", tree->lex_val->val.s);
        }

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

node_t create_node(lex_val_t val, node_type_t type){
    node_t *node = (node_t*)malloc(sizeof(node_t));
    memset(node, 0, sizeof(node_t));
    node->lex_val = val;
    node->type = type;
}

void add_child(node_t** node, node_t* child){
    int i = 0;
    while((*node)->children[i] != NULL){
        i++;
    }
    (*node)->children[i] = child;
}