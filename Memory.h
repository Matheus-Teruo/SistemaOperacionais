#ifndef MEMORY_H
#define MEMORY_H
#include <string>
#include <vector>
#include <map>
#include "MemoryTree.h"
#include "MemoryNode.h"

using namespace std;

struct logicalMemory {
  string type;
  int start;
  int total;
  int ID;
};

class Memory{
  public:
    Memory(int m);
    int Allocate(string t, MemoryTree* tree);  // return errors or position
    void Loader(string t, MemoryNode* node, int s);
    void Reallocate(string t, MemoryTree* tree, int s);
    int Unallocate(string t, MemoryTree* tree, bool k);  // return position
    void Unload(string t, MemoryNode* node, int s);
    void Show();

  private:
    int memory;
    vector<logicalMemory> Allocated;
    map<int,logicalMemory> pointers;
};

#endif