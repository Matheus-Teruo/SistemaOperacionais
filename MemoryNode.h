#ifndef MEMORYNODE_H
#define MEMORYNODE_H

using namespace std;

class MemoryNode{
  public:
    MemoryNode(int i, int m);
    MemoryNode* right;
    MemoryNode* left;
    int branch;
    int getID() const;
    int getMemory() const;
    int getNext() const;
    void setNext(int n);
    
  private:
    int ID;
    int memory;
    int next;
};

#endif