#include "Event.h"
#include "MemoryTree.h"
#include <string>

using namespace std;

Event::Event(int i, const string t, int f, MemoryTree* m, int c) : instant(i), type(t), flag(f), memory(m), cpuTime(c), status(0),next(nullptr) {};

int Event::getInstant() const{
  return instant;
};

string Event::getType() const{
  return type;
};

int Event::getFlag() const{
  return flag;
};

int Event::getStatus() const{
  return status;
};

int Event::getTotalMemory() const{
  return memory->TotalMemory();
};

int Event::getMemory() const{
  return memory->actual->getMemory();
};

int Event::getcpuTime() const {
  return cpuTime;
};

void Event::setInstant(int i){
  instant = i;
};

void Event::setFlag(int f){
  flag = f;
};

void Event::setStatus(int s){
  status = s;
};

void Event::setcpuTime(int c){
  cpuTime = c;
};