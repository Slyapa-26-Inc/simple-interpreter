//
// Created by pc on 12.05.2024.
//

#ifndef UNTITLED3_VARIABLE_TABLE_H
#define UNTITLED3_VARIABLE_TABLE_H
/*
typedef int data_t;
typedef struct variable {
    char name[21];
    data_t val;
} variable;
typedef struct var_table_node {
    variable var;
    struct var_table_node *next;
}var_table_node;
typedef struct variables_list{
    var_table_node *top, *end;
}variables_list;

void append_vartable(variables_list *list, char *name, data_t val);
void remove_by_name(variables_list *list, char *name);
*/

/* *** */

typedef int data_t;

typedef struct item {
    char *key;
    data_t val;
    struct item *next;
} item;

typedef struct hash_t {
    item **table;
    int size;
}hash_t;

item *new_item(char *key, data_t val);
hash_t *create_ht(int size);
int hash(char *key, int size);
void ht_append(hash_t *ht, char *key, data_t val);
void ht_remove(hash_t *ht, char *key);
int ht_get(hash_t *ht, char *key, data_t *dest);
int ht_rewrite(hash_t *ht, char *key, data_t new_val);
void print_ht(hash_t *ht);

#endif //UNTITLED3_VARIABLE_TABLE_H
