#include "EventList.h"
#include "EventNode.h"
#include "MemoryTree.h"
#include <string>
#include <iostream>

using namespace std;

EventList::EventList() : head(nullptr){}

void EventList::create(int i, const string t, int f, MemoryTree* m, int c){
  EventNode* newEvent = new EventNode(i, t, f, m, c);
  if (head == nullptr || head->getInstant() > i) {
    newEvent->next = head;
    head = newEvent;
    return;
  }
  EventNode* temp = head;
  while (temp->next != nullptr && temp->next->getInstant() < i){
    temp = temp->next;
  }
  newEvent->next = temp->next;
  temp->next = newEvent;
};

void EventList::insert(EventNode* e){
  int inst = e->getInstant();
  if (head == nullptr || head->getInstant() > inst) {
    e->next = head;
    head = e;
    return;
  }
  EventNode* temp = head;
  while (temp->next != nullptr && temp->next->getInstant() < inst){
    temp = temp->next;
  }
  e->next = temp->next;
  temp->next = e;
};

EventNode* EventList::takeEvent() {
  EventNode* temp = head;
  head = temp->next;
  temp->next = nullptr;
  return temp;
};

void EventList::display() const{
  EventNode* temp = head;
  cout << "\033[36m" << "Lista de Eventos===" << endl;
  while (temp != nullptr) {
    cout << "Instant:" << temp->getInstant() << ", Event:" << temp->getType();
    cout << ", Flag:" << temp->getFlag() << ", Status:" << temp->getStatus() << endl;
    temp = temp->next;
  }
  cout << "===================" << "\033[0m" << endl;
};

EventNode::EventNode(int i, const string t, int f, MemoryTree* m, int c) : instant(i), type(t), flag(f), memory(m), cpuTime(c), status(0),next(nullptr) {};

int EventNode::getInstant() const{
  return instant;
};

string EventNode::getType() const{
  return type;
};

int EventNode::getFlag() const{
  return flag;
};

int EventNode::getStatus() const{
  return status;
};

MemoryTree* EventNode::getMemoryTree() const{
  return memory;
};

int EventNode::getMemoryActual() const{
  return memory->current->getMemory();
};

int EventNode::getcpuTime() const {
  return cpuTime;
};

void EventNode::setInstant(int i){
  instant = i;
};

void EventNode::setFlag(int f){
  flag = f;
};

void EventNode::setStatus(int s){
  status = s;
};

void EventNode::setcpuTime(int c){
  cpuTime = c;
};