//
// Created by box_of_the_fox on 8/13/20.
//

#ifndef UNTITLED_LIST_H
#define UNTITLED_LIST_H

#endif //UNTITLED_LIST_H

#include <stdbool.h>
#include <gmp.h>

typedef struct smooth_vals{
    mpz_t vals[2];
} Smooth_Vals;
union dataType
{
    bool bit;
    mpz_t mpz_data;
    struct node *innerList;
    Smooth_Vals smoothVals;
    unsigned char *exp;
    unsigned long ul;
};
typedef union dataType Data;
typedef struct node
{
    Data data;
    struct node *next;
}Node;
typedef Node *List;
void ListInit(List *plist);
bool ListEmpty(const List *plist);
unsigned int ListSize(const List *plist);
bool ListAdd(Data data, List *plist);
void ListIterate(const List *plist, void (*wfun)(Data data));
void ListClear(List *plist);
bool hasNext(List plist);
Node* next(List *plist);