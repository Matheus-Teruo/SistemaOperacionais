#ifndef MEMORYTREE_H
#define MEMORYTREE_H
#include "MemoryNode.h"
#include <list>

using namespace std;

class MemoryTree{
  public:
    MemoryTree(list<int> t);
    void AuxFunc(MemoryNode* no, int segment, int MaxMOverlay);

    int getTotalMemory() const;
    int getMaxMemoryOverlay() const;
    int getTotalConditional() const;
    int getMaxSegment() const;
    void changeNode();

    MemoryNode* head;
    MemoryNode* current;
  private:
    list<int>::iterator it;
    list<int> tree;
    int maxSegment;
    int totalMemory;
    int maxMemoryOverlay;
    int totalConditions;
    int auxID;
};

#endif