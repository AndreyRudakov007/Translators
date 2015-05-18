#include <cstdlib>
#include "standart.h"


void Tree:: AddChild (int _type, double _value)
{
    if(size == 0)
    {
        childrens = (Tree**) malloc (sizeof(Tree*));
    }
    else
    {
        childrens = (Tree**) realloc (childrens, (size + 1)*sizeof(Tree*));
    }
    childrens[size] = new Tree();
    childrens[size]->type = _type;
    childrens[size]->value = _value;
    size++;
}

void Tree::AddChild (Tree *t)
{
    if(size == 0)
    {
        childrens = (Tree**) malloc (sizeof(Tree*));
    }
    else
    {
        childrens = (Tree**) realloc (childrens, (size + 1)*sizeof(Tree*));
    }
    childrens[size] = t;
    size++;
}
