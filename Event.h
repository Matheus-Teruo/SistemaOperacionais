#ifndef EVENT_H
#define EVENT_H
#include <string>

using namespace std;

class Event{
  public:
    Event(int i, const string t, int f, MemoryTree* m, int c);
    int getInstant() const;
    string getType() const;
    int getFlag() const;
    int getTotalMemory() const;
    MemoryTree* getMemoryTree() const;
    int getMemory() const;
    int getcpuTime() const;
    Event* next;

  private:
    int instant;
    string type;
    int flag;
    MemoryTree* memory;
    int cpuTime;
};

#endif