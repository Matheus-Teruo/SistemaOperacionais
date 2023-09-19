#ifndef EVENT_H
#define EVENT_H
#include <string>

using namespace std;

class Event{
  public:
    Event(int i, const string t, int f, int m, int c);
    int getInstant() const;
    string getType() const;
    int getFlag() const;
    int getMemory() const;
    int getcpuTime() const;
    Event* next;

  private:
    int instant;
    string type;
    int flag;
    int memory;
    int cpuTime;
};

#endif