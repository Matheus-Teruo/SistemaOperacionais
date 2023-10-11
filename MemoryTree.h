#ifndef MEMORYTREE_H
#define MEMORYTREE_H
#include "MemoryNode.h"
#include <list>

using namespace std;

class MemoryTree{
  public:
    MemoryTree(list<int> tree);
    void AuxFunc(list<int> tree, MemoryNode* no, int segment);
    int getTotalMemory() const;
    int getTotalConditional() const;
    int getMaxSegment() const;

    MemoryNode* head;
    MemoryNode* actual;
  private:
    int maxSegment;
    int totalMemory;
    int totalConditions;
};

#endif