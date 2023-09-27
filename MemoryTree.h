#ifndef MEMORYTREE_H
#define MEMORYTREE_H
#include <list>

using namespace std;

class MemoryTree{
  public:
    MemoryTree(list<int> tree);
    void AuxFunc(list<int> tree, MemoryNode* no);
    int TotalMemory() const;
    int TotalConditional() const;

    MemoryNode* head;
    MemoryNode* actual;
  private:
    int totalmemory;
    int totalconditions;
};

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