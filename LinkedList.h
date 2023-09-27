#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include "Event.h"
#include <string>

using namespace std;

class LinkedList{
  public:
    Event* head;
    LinkedList();

    void insert(int i, const string t, int f, MemoryTree m, int c);
    Event* takeEvent();
    void display() const;
};

#endif