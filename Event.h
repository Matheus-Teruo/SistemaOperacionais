#ifndef EVENT_H
#define EVENT_H
#include "MemoryTree.h"
#include <string>

using namespace std;

class Event{
  public:
    Event(int i, const string t, int f, MemoryTree* m, int c);
    int getInstant() const;
    string getType() const;
    int getFlag() const;
    int getStatus() const;
    int getTotalMemory() const;
    int getMemory() const;
    int getcpuTime() const;

    void setInstant(int i);
    void setFlag(int f);
    void setStatus(int s);
    void setcpuTime(int c);
    Event* next;

  private:
    int instant;
    string type;
    int flag;
    int status;
    MemoryTree* memory;
    int cpuTime;
};

#endif