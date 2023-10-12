#ifndef MEMORYTREE_H
#define MEMORYTREE_H
#include "MemoryNode.h"
#include <list>

using namespace std;

class MemoryTree{
  public:
    MemoryTree(list<int> t);
    void AuxFunc(MemoryNode* no, int segment);
    int getTotalMemory() const;
    int getTotalConditional() const;
    int getMaxSegment() const;

    MemoryNode* head;
    MemoryNode* actual;
  private:
    list<int>::iterator it;
    list<int> tree;
    int maxSegment;
    int totalMemory;
    int totalConditions;
};

#endif