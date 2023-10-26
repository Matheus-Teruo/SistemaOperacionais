#ifndef MEMORYNODE_H
#define MEMORYNODE_H

using namespace std;

class MemoryNode{
  public:
    MemoryNode(int i, int m, int t, int d, int io1, int io2);
    MemoryNode* right;
    MemoryNode* left;
    int branch;
    int getID() const;
    int getMemory() const;
    int getcpuTimeSeg() const;
    int getNext() const;
    void setNext(int n);
    
  private:
    int ID;
    int memory;
    int CPUTimeSeg;
    int disk;
    int device1;
    int device2;
    int next;
};

#endif