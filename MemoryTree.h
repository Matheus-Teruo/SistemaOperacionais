#ifndef MEMORYTREE_H
#define MEMORYTREE_H
#include "MemoryNode.h"
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

#endif