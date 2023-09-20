#ifndef MEMORY_H
#define MEMORY_H
#include <string>
#include <vector>

using namespace std;

struct logicalMemory {
    string type;
    int start;
    int total;
};

class Memory{
  public:
    Memory(int m);
    int Loader(string t, int p);
    bool Unload(string t, int p);
    void Show();

  private:
    int memory;
    vector<logicalMemory> pointers;
};

#endif