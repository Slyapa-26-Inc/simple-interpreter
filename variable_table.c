//
// Created by pc on 12.05.2024.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "variable_table.h"

/*
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
*/

item *new_item(char *key, data_t val)
{
    item *new = (item*) malloc(sizeof(item));
    new->key = strdup(key);
    new->val = val;
    new->next = NULL;
    return new;
}

hash_t *create_ht(int size)
{
    hash_t *ht = (hash_t *) malloc(sizeof(hash_t));
    ht->table = (item**) malloc(size * sizeof(item*));
    ht->size = size;
    for (int i = 0; i < size; ++i) {
        ht->table[i] = NULL;
    }
    return ht;
}

int hash(char *key, int size)
{
    int h= 0, a = 127;
    for (; *key != 0; key++)
        h = (a*h + *key) % size;
    return (int)h;
}

void ht_append(hash_t *ht, char *key, data_t val)
{
    int index = hash(key,ht->size);
    item *elem = new_item(key, val);
    if (ht->table[index] == NULL)
    {
        ht->table[index] = elem;
    } else {
        item *tmp = ht->table[index];
        while (1) {
//            printf("%s - %s %d\n", key, tmp->key, strcmp(tmp->key, key));
            if (strcmp(tmp->key, key) == 0)
            {
                free(elem->key);
                free(elem);
                return;
            }
            if (tmp->next == NULL)
                break;
            tmp = tmp->next;
        }
        tmp->next = elem;
    }
}

void ht_remove(hash_t *ht, char *key)
{
    int index = hash(key, ht->size);
    item *tmp = ht->table[index];
    item *prev = NULL;
    while (tmp != NULL && strcmp(tmp->key, key) != 0)
    {
        prev = tmp;
        tmp = tmp->next;
    }
    if (tmp == NULL)
        return;
    if (prev == NULL)
        ht->table[index] = ht->table[index]->next;
    else
        prev->next = tmp->next;
    free(tmp->key);
    free(tmp);
}

int ht_get(hash_t *ht, char *key, data_t *dest)
{
    int index = hash(key, ht->size);
    item *tmp = ht->table[index];
    while (tmp)
    {
        if (strcmp(tmp->key, key) == 0) {
            *dest = tmp->val;
            return 0;
        }
    }
    return -1;
}

int ht_rewrite(hash_t *ht, char *key, data_t new_val)
{
    int index = hash(key, ht->size);
    item *tmp = ht->table[index];
    while (tmp)
    {
        if (strcmp(tmp->key, key) == 0) {
            tmp->val = new_val;
            return 0;
        }
    }
    return -1;
}

void print_ht(hash_t *ht)
{
    for (int i = 0; i < ht->size; ++i) {
        printf("[%d] ", i);
        if (ht->table[i] == NULL)
            printf("NULL");
        else {
            item *tmp = ht->table[i];
            while (tmp != NULL)
            {
                printf("{\"%s\", %d} ", tmp->key, tmp->val);
                tmp = tmp->next;
            }
        }
        printf("\n");

    }
}