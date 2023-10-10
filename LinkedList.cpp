#include "LinkedList.h"
#include "MemoryTree.h"
#include "Event.h"
#include <string>
#include <iostream>

using namespace std;

LinkedList::LinkedList() : head(nullptr){}

void LinkedList::create(int i, const string t, int f, MemoryTree& m, int c){
  Event* newEvent = new Event(i, t, f, &m, c);
  if (head == nullptr || head->getInstant() > i) {
    newEvent->next = head;
    head = newEvent;
    return;
  }
  Event* temp = head;
  while (temp->next != nullptr && temp->next->getInstant() < i){
    temp = temp->next;
  }
  newEvent->next = temp->next;
  temp->next = newEvent;
};

void LinkedList::insert(Event* e){
  int inst = e->getInstant();
  if (head == nullptr || head->getInstant() > inst) {
    e->next = head;
    head = e;
    return;
  }
  Event* temp = head;
  while (temp->next != nullptr && temp->next->getInstant() < inst){
    temp = temp->next;
  }
  e->next = temp->next;
  temp->next = e;
};

Event* LinkedList::takeEvent() {
  Event* temp = head;
  head = temp->next;
  return temp;
};

void LinkedList::display() const{
  Event* temp = head;
  while (temp != nullptr) {
    cout << "Instant:" << temp->getInstant() << ", Event:" << temp->getType() << endl;
    temp = temp->next;
  }
};