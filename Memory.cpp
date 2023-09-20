#include "Memory.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

Memory::Memory(int m): memory(m){};

int Memory::Loader(string t, int p){
  if (memory < p){
    return -2; // programa não cabe na memória
  }
  int startspace = 0;
  int endspace;
  auto location = pointers.begin();
  while (location != pointers.end()){
    endspace = location->start;
    if ((endspace - startspace) >= p){
      logicalMemory program = {t,startspace,p};
      pointers.insert(location,program);
      return startspace;
    }
    startspace = location->total + location->start + 1;
    ++location;
  };
  endspace = memory;
  if ((endspace - startspace) >= p){
    logicalMemory program = {t,startspace,p};
    pointers.insert(location,program);
    return startspace;
  }
  return -1;
};

bool Memory::Unload(string t,int p){
  auto iterator_pointers = remove_if(pointers.begin(), pointers.end(), [t](const logicalMemory& lm) {return lm.type == t;});
  if (iterator_pointers != pointers.end()){
    pointers.erase(iterator_pointers);
    return true;
  } else {
    return false;
  }
};


void Memory::Show(){
  for (const auto& lm : pointers) {
    cout << "Type: " << lm.type << ", Start: " << lm.start << ", Total: " << lm.total << endl;
  }
}