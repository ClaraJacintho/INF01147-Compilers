#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data.h"

extern void exporta (void *arvore);
extern void libera (void *arvore);

lex_val_t* get_lit_lex_val(int line, val_t v, token_t t);
lex_val_t* get_lex_val(int line, token_t type, char *name);

void print_edges(void *arvore);
void print_edge(node_t *node);
void print_node_labels(void *arvore);
void print_label(node_t* node);

void free_nodes(node_t *node);
void free_lex_val(lex_val_t* lex_val);

node_t* insert_node_next(node_t** n1, node_t* n2);
node_t* create_node(lex_val_t* val, node_type_t type);
void add_child(node_t** node, node_t* child);
type_t type_inference(type_t a, type_t b);

node_t* create_function_declaration(node_t* header, node_t* commands);
node_t* create_node_literal(lex_val_t* val, node_type_t node_type);
node_t *create_node_declared_identifier(lex_val_t *val, node_type_t node_type, kind_t k);
node_t* create_vector_node(node_t* id, node_t* exp);
node_t* create_init_node(node_t* id, lex_val_t* lv, node_t* val);
node_t* create_attrib_node(node_t* id, lex_val_t* lv, node_t* val);
node_t *create_ternop_node(node_t *opA, node_t *opB, node_t *opC);
node_t* create_binop_node(node_t* opA, lex_val_t* lv, node_t* opB);
node_t *create_unop_node(lex_val_t *lv, node_t *opA);
node_t* create_input_node(node_t* id);
node_t* create_output_node(node_t* var);
node_t* create_func_call_node(lex_val_t* lv, node_t* args);
node_t *create_shift_node(node_t *id, lex_val_t *lv, node_t *val);
node_t *create_return_node(node_t* ex);
node_t *create_cmd_node(node_type_t node_type);

int is_convertible_type(type_t t);
void update_node_type(node_t *node, type_t t);
node_t* update_node_init(node_t *node, type_t t);

#endif