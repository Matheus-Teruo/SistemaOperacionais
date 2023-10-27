#include "EventList.h"
#include "EventNode.h"
#include "MemoryTree.h"
#include <string>
#include <iostream>

using namespace std;

EventList::EventList() : head(nullptr){}

void EventList::create(int i, const string t, MemoryTree* m, int c){
  EventNode* newEvent = new EventNode(i, t, m, c);
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
  bool auxOfPrint = false;
  if (temp == nullptr){
    auxOfPrint = true;
    cout << "\033[96m" << "lista de eventos ===" << endl;
  } else {
    cout << "\033[96m" << "lista de eventos ==========================" << endl;
  }
  while (temp != nullptr) {
    cout << "Instant:" << temp->getInstant() << ", Event:" << temp->getType();
    cout << ", Flag:" << temp->getFlag() << ", Status:" << temp->getStatus();
    cout << ", Printer:" << temp->getAllocation().device1;
    cout << ", Scanner:" << temp->getAllocation().device2 << endl;
    temp = temp->next;
  }
  if (auxOfPrint){
    cout << "====================" << "\033[0m" << endl;
  } else {
    cout << "===========================================" << "\033[0m" << endl;
  }
  
};

EventNode::EventNode(int i, const string t, MemoryTree* m, int c) : 
instant(i), type(t), flag(1), memory(m), cpuTime(c), status(0),next(nullptr) {
  int aux1 = 0;
  int aux2 = 0;
  if (m->getUseDevice1() == 0){
    aux1 = -1;
  }
  if (m->getUseDevice2() == 0){
    aux2 = -1;
  }
  allocation = logicalAllocation{aux1,aux2};
};

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

MemoryNode* EventNode::getCurrentMemoryNode() const{
  return memory->current;
};

logicalAllocation EventNode::getAllocation() const{
  return allocation;
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

void EventNode::setAlDevice1(int a){
  allocation.device1 = a;
};

void EventNode::setAlDevice2(int a){
  allocation.device2 = a;
};

void EventNode::setcpuTime(int c){
  cpuTime = c;
};