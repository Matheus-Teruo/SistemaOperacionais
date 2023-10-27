#ifndef MEMORYTREE_H
#define MEMORYTREE_H
#include "MemoryNode.h"
#include <list>

using namespace std;

class MemoryTree{
  public:
    MemoryTree(list<int> t);

    int getTotalMemory() const;
    int getMaxMemoryOverlay() const;
    int getTotalConditional() const;
    int getUseDevice1() const;
    int getUseDevice2() const;
    void changeNode();

    MemoryNode* head;
    MemoryNode* current;
  private:
    void AuxFunc(MemoryNode* no, int segment, int MaxMOverlay);
    list<int>::iterator it;
    list<int> tree;
    int totalMemory;
    int maxMemoryOverlay;
    int totalConditions;
    int useDevice1;
    int useDevice2;
    int auxID;
};

#endif