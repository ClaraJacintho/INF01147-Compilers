#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

extern  void exporta (void *arvore);
extern void libera (void *arvore);

lex_val_t *get_lit_lex_val(int line, val_t v);
lex_val_t *get_lex_val(int line, token_t type, char *name);

void print_edges(void *arvore);
void print_edge(node_t *node);
void print_node_labels(void *arvore);
void print_label(node_t* node);

node_t* insert_command_node(node_t** n1, node_t *n2);
node_t* create_node(lex_val_t *val, node_type_t type);
void add_child(node_t** node, node_t* child);

#endif