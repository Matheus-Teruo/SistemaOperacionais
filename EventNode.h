#ifndef EVENTNODE_H
#define EVENTNODE_H
#include "MemoryTree.h"
#include <string>

using namespace std;

class EventNode{
  public:
    EventNode(int i, const string t, MemoryTree* m, int c);

    int getInstant() const;
    string getType() const;
    int getFlag() const;
    int getStatus() const;
    MemoryTree* getMemoryTree() const;
    MemoryNode* getCurrentMemoryNode() const;
    int getcpuTime() const;

    void setInstant(int i);
    void setFlag(int f);
    void setStatus(int s);
    void setcpuTime(int c);
    EventNode* next;

  private:
    int instant;
    string type;
    int flag;
    int status;
    MemoryTree* memory;
    int cpuTime;
};

#endif