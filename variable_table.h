//
// Created by pc on 12.05.2024.
//

#ifndef UNTITLED3_VARIABLE_TABLE_H
#define UNTITLED3_VARIABLE_TABLE_H

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

#endif //UNTITLED3_VARIABLE_TABLE_H
