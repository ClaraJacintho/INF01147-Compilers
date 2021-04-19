#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "symbol_table.h"
#include "error_handling.h"
#include "code_generation.h"


/**
 * Creates a lexical value from a literal
 * @param line declaration line
 * @param v the value
 * @param t the token type of the value
 * @return a lex val
 */

lex_val_t *get_lit_lex_val(int line, val_t v, token_t t){
    lex_val_t *lex = (lex_val_t*)malloc(sizeof(lex_val_t));
    lex->line = line;
    lex->type = t;
    lex->val  = v;
    return lex; 
}

/**
 * Creates a lexical value for an identifier or special char
 * @param line declaration line
 * @param v the value
 * @param t the token type of the value
 * @return a lex val
 */
lex_val_t *get_lex_val(int line, token_t type, char *name){
    lex_val_t *lex = (lex_val_t*)malloc(sizeof(lex_val_t));
    lex->line = line;
    lex->type = type;
    lex->val.name = name;
    return lex; 
}

void exporta (void *arvore) {
    // print_edges(arvore);
    // print_node_labels(arvore);
    operation_t* ops = concat_code(init(), ((node_t*)arvore)->code);
    print_code(ops);
}

void libera (void *arvore){
    free_nodes((node_t*)arvore);
    free_all_scopes();
}

/**
 *  Frees a node and it's children recursively
 * @param node the root of the tree that will be freed 
 */
void free_nodes(node_t *node){
    int i;
    if (node != NULL){
        for(i = 0; i < MAX_CHILDREN; i++){
            if(node->children[i] != NULL) {
                free_nodes(node->children[i]);
            }
        }

        if(node->next != NULL){
            free_nodes(node->next);
        }
        
        if(node->lex_val != NULL){
            
            if(node->lex_val->type == SP_CHAR || node->lex_val->type == OP || node->lex_val->type == DECL_ID){
                //printf("FREEING %s\n", node->lex_val->val.s);
                free(node->lex_val->val.s);
                free(node->lex_val);
            }
            
            
        }
        
        free(node);
    }
}

/**
 * Frees an intentifier or special char's lex val
 *  @param lex_val a pointer to the value that'll be freed
 */
void free_lex_val(lex_val_t* lex_val){
    if(lex_val != NULL){
            free(lex_val->val.s);
            free(lex_val);
    }
}

/**
 * Recursively print the edges of an AST
 * @param arovere root of the tree
 */
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

/**
 * Recursively print the node labels of an AST
 * @param arvore root of the tree
 */
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
        switch(node->node_type){
            case IN: printf("input"); break;
            case OUT: printf("output"); break;
            case INIT:
            case ATTR:
            case SHIFT:
            case IDENT:
            case UN_OP:
            case BIN_OP:
            case FUNC:
                printf("%s", node->lex_val->val.name);
                break;
            case LIT_BOOL:
                if (node->lex_val->val.b){
                    printf("true");
                } else {
                    printf("false");
                }
                break;
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
            case VECTOR:
                printf("[]");
                break;
            case FUNC_CALL:
                printf("call %s", node->lex_val->val.name);
                break;
            case TERN_OP:
                printf("?:");
                break;
            case RETURN:
                printf("return");
                break;
            case IF:
                printf("if");
                break;
            case FOR:
                printf("for");
                break;
            case WHILE:
                printf("while");
                break;
            case NOT_INIT:
                printf("ERROR");
                break;

            default:
                printf("%i", node->node_type);
        }
        printf("\"];\n");
        //for debugging node types
        //printf(" type = %s\"];\n", get_type_name(node->type));
    }
    
}

/**
 * Recursively print the node labels of an AST
 * @param arvore root of the tree
 */
node_t* create_node(lex_val_t *val, node_type_t type){
    node_t *node = (node_t*)malloc(sizeof(node_t));
    memset(node, 0, sizeof(node_t));
    node->lex_val = val;
    node->node_type = type;

    node->code = NULL;
    node->patch_true = NULL;
    node->patch_false = NULL;

    return node;
}

/**
 * Inserts a node as "next" from another node;
 * If either node is a declaration without initialization,
 * it will not be added and will be freed instead;
 * n1 may be a list of nodes - if so, it will find the last node 
 * on the list and add n2 as "next" of that node;
 * 
 * @param n1 list of nodes  
 * @param n2 node that will be added as next
 */
node_t* insert_node_next(node_t** n1, node_t *n2){
    node_t* aux2;
    if(*n1 != NULL && (*n1)->node_type != NOT_INIT){
        while(n2 != NULL && n2->node_type == NOT_INIT){
            aux2 = n2;
            n2 = n2->next; // forward untill the next attribuition
            free(aux2);
        }

        // get last of list:
        node_t* aux = (*n1);
        if((*n1)->next != NULL){
            while(aux->next != NULL){
                aux = aux->next;
            }
            aux->next = n2;
        } else {
            (*n1)->next = n2;
        }
        if(n2 != NULL){
            (*n1)->code = concat_code((*n1)->code, n2->code);
        }
        
        return *n1;
    } else {
        if(*n1 != NULL){
            free(*n1);;
        }
        return n2;
    }
}


/**
 * Inserts a child node
 * @param node pointer to parent
 * @param child child node
 */
void add_child(node_t** node, node_t* child){
    int i = 0;
    while((*node)->children[i] != NULL){
        i++;
    }
    (*node)->children[i] = child;
}

/**
 * Infers a type from two types. Float has precedence over all,
 * then int. Bool is only returned if both types are bool;
 * @param a type
 * @param b type
 * @returns a type
 */
type_t type_inference(type_t a, type_t b){
    if(!is_convertible_type(a))
        return a;
    if(a == TYPE_FLOAT || b == TYPE_FLOAT)
        return TYPE_FLOAT;
    
    if(a == TYPE_INT || b == TYPE_INT)
        return TYPE_INT;
    
    return TYPE_BOOL;
}

node_t* create_function_declaration(node_t* header, node_t* commands){
    add_child(&header, commands);
    operation_t* func_declaration_code = gen_function_declaration(header);
    header->code = concat_code(func_declaration_code, commands->code);
    return header;
}

/**
 * Creates an AST node for a literal and inserts it in the symbol table;
 * @param val the literal's lexical value
 * @param node_type what kind of literal it is
 * @returns a node
 */
node_t* create_node_literal(lex_val_t *val, node_type_t node_type){
    node_t *node = create_node(val, node_type);
    node->type = get_type(val->type);
    node->reg = gen_reg();
    node->code = gen_literal(node);
    insert_literal(val);
    return node;
}


/**
 * Creates an AST node for an identifier that should
 * have been declared;
 * @param val the identifier's lexical value
 * @param node_type what kind of identifier it is
 * @param kind_t if it is an id or a vector
 * @returns a node
 */
node_t *create_node_declared_identifier(lex_val_t *val, node_type_t node_type, kind_t k){
    /** 
     * we created a special token type for declared identifiers so their 
    * lexical values could be freed with its node, since it will not be on the 
    * symbol table.
    */
    val->type = DECL_ID; // haxx
    node_t *node = create_node(val, node_type);
    symbol_t* s = find_identifier_symbol(val, k);
    if(s != NULL){
        if(s->kind == k){
            node->type = s->type;
        } else {
            throw_kind_error(val->line, get_key(val), s->kind, k);
        }
        node->type = s->type;
    } else {
        throw_undeclared_error(val->line, get_key(val));
    }
    return node;
}

node_t* create_vector_node(node_t* id, node_t* exp){
    node_t* node = create_node(NULL, VECTOR);
    node->type = id->type;
    add_child(&node, id); 
    add_child(&node, exp);
    return node;
}

node_t *create_init_node(node_t *id, lex_val_t *lv, node_t *val){
    node_t* node = create_node(lv, INIT); 
    add_child(&node, id);
    add_child(&node, val);
    
    return node;
}


/**
 * Recursively updates initialization node with the correct type;
 * @param node init node
 * @param type new node type
 */
void update_node_init(node_t *node, type_t t){
    if(node == NULL)
        return;
    if(node->node_type == NOT_INIT){
        update_node_init(node->next, t);
        return;
    }
    node->type = t;

    // the identifier
    if(node->children[0] != NULL)
        node->children[0]->type = t;
    
    // the value being attributed
    node_t* child = node->children[1];

    // types must be compatible
    if(child->type != t){
        if(!is_convertible_type(t) || !is_convertible_type(child->type))
            throw_wrong_type_error(child->lex_val->line, node->children[0]->lex_val->val.s, t, child->type);
    }
    node->children[1]->type = t;
    node->code = concat_code(node->children[1]->code, gen_init(node->children[0], node->children[1]));
    update_node_init(node->next, t);

}

int is_convertible_type(type_t t){
    if((t == TYPE_BOOL || t == TYPE_INT || t == TYPE_FLOAT ))
        return 1;
    return 0;
}


node_t *create_attrib_node(node_t *id, lex_val_t *lv, node_t *val){
    kind_t k = id->node_type == VECTOR ? K_VEC : K_ID;
    node_t* node = create_node(lv, ATTR); 

    // types must be compatible
    if(id->type != val->type){
        if(!is_convertible_type(id->type) || !is_convertible_type(val->type)){
            lex_val_t* lv;
            if(id->node_type == VECTOR){
                lv = id->children[0]->lex_val;
            } else {
                lv = id->lex_val;
            }
            throw_wrong_type_error(lv->line, lv->val.s, id->type, val->type);
        }
    }

    // if id is string, checks if received val exceeds string size
    if(id->type == TYPE_STRING && (val->node_type == IDENT || val->node_type == LIT_STR)){ // prof, não tem como pegar o tamanho de um no de comando, por exemplo, né? 
        int size = get_size_from_identifier(val->lex_val);
        symbol_t* s = find_identifier_symbol(id->lex_val, k);
        printf("qsldkfjnsqlkjfnqslkdjdf\n");
        if(size > s->size){
            throw_string_max_error(s, size);
        }
    }

    add_child(&node, id);
    node->type = type_inference(id->type, val->type);
    add_child(&node, val);
    return node;
}

void update_node_type(node_t *node, type_t t){
    if (node != NULL)
        node->type = t;
}

node_t *create_ternop_node(node_t *opA, node_t *opB, node_t *opC){
    node_t* node = create_node(NULL, TERN_OP); 

    if(opB->type != opC->type){
        if(!is_convertible_type(opB->type) || !is_convertible_type(opC->type)){
            if(!is_convertible_type(opC->type)){
                throw_string_char_to_x_error(opC->lex_val->line, get_key(opC->lex_val), opC->type, opB->type);
            }
            if(!is_convertible_type(opB->type)){
                throw_string_char_to_x_error(opB->lex_val->line, get_key(opB->lex_val), opB->type, opC->type);
            }
        }
    }
    add_child(&node, opA);
    node->type = type_inference(opB->type, opC->type);
    add_child(&node, opB);
    add_child(&node, opC);
    return node;
}

node_t *create_binop_node(node_t *opA, lex_val_t *lv, node_t *opB){
    node_t* node = create_node(lv, BIN_OP); 
    add_child(&node, opA);
    node->type = type_inference(opA->type, opB->type);
    add_child(&node, opB);
     if(opB->type != opA->type){
        if(!is_convertible_type(opB->type) || !is_convertible_type(opA->type)){
            if(!is_convertible_type(opB->type)){
                throw_string_char_to_x_error(opB->lex_val->line, get_key(opB->lex_val), opB->type, opA->type);
            } 
            if(!is_convertible_type(opA->type)){
                throw_string_char_to_x_error(opA->lex_val->line, get_key(opA->lex_val), opA->type, opB->type);
            }
        }
    }

    return node;
}

node_t *create_unop_node(lex_val_t *lv, node_t *opA){
    node_t* node = create_node(lv, UN_OP); 
    add_child(&node, opA);
    node->type = opA->type;
    return node;
}


node_t* create_input_node(node_t* id){
    node_t* node = create_node(NULL, IN); 
    if(id->type != TYPE_INT && id->type != TYPE_FLOAT){
        throw_input_par_error(id->lex_val->line, id->type);
    }
    add_child(&node, id);
    node->type = id->type;
    return node;
}

node_t* create_output_node(node_t* var){
    node_t* node = create_node(NULL, OUT); 
    if(var->type != TYPE_INT && var->type != TYPE_FLOAT){
        throw_output_par_error(var->lex_val->line, var->type);
    }
    add_child(&node, var);
    node->type = var->type;
    return node;
}


/**
 * Creates an AST node for a function call;
 * Finds the function's arguments from the symbol table; 
 * @param lex_val the function lex val
 * @param args received arguments
 * @returns a node
 */
node_t* create_func_call_node(lex_val_t *lv, node_t *args){
    lv->type = DECL_ID; //haxxs
    node_t* node = create_node(lv, FUNC_CALL); 
    add_child(&node, args);
    symbol_t* s = find_identifier_symbol(lv, K_FUNC);

    if(s != NULL){
        // identifier must be a function
        if(s->kind == K_FUNC){
            node->type = s->type;
        } else {
            throw_kind_error(lv->line, get_key(lv), s->kind, K_FUNC);
        }
        
    } else {
        throw_undeclared_error(lv->line, get_key(lv));
    }

    symbol_table_item_t* declared_param = s->args;
    node_t* received_arg = args;
    // goes through each expected argument to match it with a received argument
    for(int i = 0; i < s->n_args; i++){

        // if there are no more received arg, there were not enough args
        if(received_arg == NULL){
            throw_missing_args_error(lv->line, s->key, s->n_args, i);
        }

        // types must be compatible
        if(declared_param->item->type != received_arg->type){
            if(!is_convertible_type(declared_param->item->type) || !is_convertible_type(received_arg->type)){
                if(received_arg->type == TYPE_STRING){
                    throw_func_string_error(lv->line);
                } else {
                    throw_wrong_type_args_error(lv->line, s->key, declared_param->item->key, declared_param->item->type, received_arg->type);
                }
                
            }
        }
        declared_param = declared_param->next;
        received_arg = received_arg->next;
    }

    // if we have not gone through all received arguments, there were too many args
    if(received_arg != NULL){
        throw_excess_args_error(lv->line, s->key, s->n_args);
    }
    
    return node;
}

node_t *create_shift_node(node_t *id, lex_val_t *lv, node_t *val){
    node_t* node = create_node(lv, SHIFT);

    // shift value must be less than 16
    if(val->lex_val->val.n > 16){
        throw_shift_par_error(val->lex_val->line, val->lex_val->val.n);
    }

    symbol_t* s;
    if(id->node_type==VECTOR){
        node_t* ident = id->children[0];
        s = find_symbol(ident->lex_val);
    } else {
        s = find_symbol(id->lex_val);
    }
        
    if(s != NULL){
        add_child(&node, id);
        node->type = id->type;
    }
    add_child(&node, val);
    return node;
}

node_t *create_return_node(node_t* ex){
    node_t* node = create_node(NULL, RETURN); 
    node->type = ex->type;
    symbol_t* func = get_current_function();
    if(ex->type == TYPE_STRING){
            throw_func_string_error(ex->lex_val->line);
    }

    // return type must match function type
    if(func->type != ex->type){
        if(!is_convertible_type(func->type) || !is_convertible_type(ex->type)){
            throw_return_par_error(ex->lex_val->line,func->type, ex->type);
        }
    }
    add_child(&node, ex); 
    return node;
}

node_t *create_cmd_node(node_type_t node_type){
    node_t* node = create_node(NULL, node_type); 
    node->type = TYPE_CMD;
    return node;
}