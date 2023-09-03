#ifndef EVENT_H
#define EVENT_H
#include <string>

using namespace std;

class Event{
  public:
    Event(int i, const string t, int m, int c);
    int getInstant() const;
    string getType() const;
    int getMemory() const;
    int getcpuTime() const;

  private:
    int instant;
    string type;
    int memory;
    int cpuTime;
};

#endif