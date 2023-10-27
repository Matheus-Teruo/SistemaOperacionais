#ifndef EVENTNODE_H
#define EVENTNODE_H
#include "MemoryTree.h"
#include <string>

using namespace std;

struct logicalAllocation{
  int device1;
  int device2;
};

class EventNode{
  public:
    EventNode(int i, const string t, MemoryTree* m, int c);

    int getInstant() const;
    string getType() const;
    int getFlag() const;
    int getStatus() const;
    MemoryTree* getMemoryTree() const;
    MemoryNode* getCurrentMemoryNode() const;
    logicalAllocation getAllocation() const;
    int getcpuTime() const;

    void setInstant(int i);
    void setFlag(int f);
    void setStatus(int s);
    void setAlDevice1(int a);
    void setAlDevice2(int a);
    void setcpuTime(int c);
    EventNode* next;

  private:
    int instant;
    string type;
    int flag;
    int status;
    MemoryTree* memory;
    logicalAllocation allocation;
    int cpuTime;
};

#endif