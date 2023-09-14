#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include "Event.h"
#include <string>

using namespace std;

class LinkedList{
  public:
    Event* head;
    LinkedList();

    void insertEmergency(int i, const string t, int m, int c);
    void insert(int i, const string t, int m, int c);
    Event* takeEvent();
    void display() const;
};

#endif