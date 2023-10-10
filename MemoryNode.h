#ifndef MEMORYNODE_H
#define MEMORYNODE_H

using namespace std;

class MemoryNode{
  public:
    MemoryNode(int m);
    MemoryNode* right;
    MemoryNode* left;
    int getMemory() const;
  private:
    int memory;
};

#endif