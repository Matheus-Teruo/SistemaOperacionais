#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include "Event.h"
#include "MemoryTree.h"
#include <string>

using namespace std;

class LinkedList{
  public:
    Event* head;
    LinkedList();

    void create(int i, const string t, int f, MemoryTree& m, int c);
    void insert(Event* e);
    Event* takeEvent();
    void display() const;
};

#endif