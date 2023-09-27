#include "Event.h"
#include "MemoryTree.h"
#include <string>

using namespace std;

Event::Event(int i, const string t, int f, MemoryTree m, int c) : instant(i), type(t), flag(f), memory(&m), cpuTime(c), next(nullptr) {};

int Event::getInstant() const{
  return instant;
};

string Event::getType() const{
  return type;
};

int Event::getFlag() const{
  return flag;
}

int Event::getTotalMemory() const{
  return memory->TotalMemory();
};

MemoryTree* Event::getMemoryTree() const{
  return memory;
};

int Event::getMemory() const{
  return memory->actual->getMemory();
};

int Event::getcpuTime() const {
  return cpuTime;
};