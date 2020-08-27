#include <stdlib.h>
#include "list.h"
static void CopyToNode(Data data, Node *pnode);
void ListInit(List *plist)
{
    *plist = NULL;
}
bool ListEmpty(const List *plist)
{
    if(*plist == NULL)
        return true;
    else
        return false;
}
unsigned int ListSize(const List *plist)
{
    unsigned int licznik = 0;
    Node *w = *plist;
    while(w != NULL)
    {
        ++licznik;
        w=w->next;
    }
    return licznik;
}
bool ListAdd(Data data, List *plist)
{
    Node *pnew;
    Node *seeked=*plist;
    pnew=(Node *)malloc(sizeof(Node));
    if(pnew == NULL)
        return false;
    CopyToNode(data, pnew);
    pnew->next=NULL;
    if(seeked == NULL)
        *plist=pnew;
    else
    {
        while(seeked->next != NULL)
            seeked=seeked->next;
        seeked->next=pnew;
    }
    return true;
}
void ListIterate(const List *plist, void (*wfun)(Data data))
{
    Node *pNode= *plist;
    while(pNode != NULL)
    {
        (*wfun)(pNode->data);
        pNode=pNode->next;
    }
}
void ListClear(List *plist)
{
    Node *pNode;
    while(*plist != NULL)
    {
        pNode = (*plist)->next;
        free(*plist);
        *plist=pNode;
    }
}
static void CopyToNode(Data data, Node *pnode)
{
    pnode->data=data;
}

bool hasNext(List plist){
    return plist != NULL;
}

Node* next(List *plist){
    return (*plist)->next;
}