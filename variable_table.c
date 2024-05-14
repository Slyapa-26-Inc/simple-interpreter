//
// Created by pc on 12.05.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "variable_table.h"


void append_vartable(variables_list *list, char *name, data_t val)
{
    var_table_node *tmp = (var_table_node *) malloc(sizeof(var_table_node));
    strncpy(tmp->var.name, name, 20);
    tmp->var.val = val;
    tmp->next = NULL;
    if (list->top == NULL && list->end == NULL) {
        list->top = list->end = tmp;
        return;
    }
    list->end->next = tmp;
    list->end = tmp;
}

void remove_by_name(variables_list *list, char *name)
{
    var_table_node *t = list->top;
    var_table_node *tmp;
    if (strcmp(list->top->var.name, name) == 0) {
        tmp = list->top;
        t = list->top = list->top->next;
        free(tmp);
    }


    while (t != NULL) {
        if (strcmp(t->next->var.name, name) == 0) {
            tmp = t->next;
            t->next = t->next->next;
            free(tmp);
            return;
        }
        t = t->next;
    }
}
