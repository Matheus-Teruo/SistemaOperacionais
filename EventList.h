#ifndef EVENTLIST_H
#define EVENTLIST_H
#include "EventNode.h"
#include "MemoryTree.h"
#include <string>

using namespace std;

class EventList{
  public:
    EventNode* head;
    EventList();

    void create(int i, const string t, int f, MemoryTree* m, int c);
    void insert(EventNode* e);
    EventNode* takeEvent();
    void display() const;
};

#endif