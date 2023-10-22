#include "Memory.h"
#include "MemoryNode.h"
#include "RandomFunction.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

int invertInt(int A){
  int B = 0;
  while (A > 0) {
    int C = A % 10;
    B = B * 10 + C;
    A /= 10;
  }
  return B;
}

int countDigits(int num) {
    int count = 0;
    while (num != 0) {
        num = num / 10;
        count++;
    }
    return count;
}

Memory::Memory(int m): memory(m){};

int Memory::Allocate(string t, MemoryTree* tree){
  if (memory < (tree->getMaxMemoryOverlay() + 5)){return -2;}
  int startspace = 0;
  int endspace;
  int status;
  auto location = Allocated.begin();
  int total = tree->getMaxMemoryOverlay() + 5;
  while (location != Allocated.end()){
    endspace = location->start;
    if ((endspace - startspace) >= total){
      logicalMemory program = logicalMemory{t,startspace,total, 0};
      cout << "alocou dessa forma: " << t << endl;
      Allocated.insert(location, program);
      Loader(t, tree->head, startspace);
      return startspace;
    }
    startspace = location->total + location->start + 1;
    ++location;
    cout << "location +1" << endl;
  };
  endspace = memory;
  if ((endspace - startspace) >= total){
    logicalMemory program = logicalMemory{t,startspace,total};
    cout << "alocou assim: " << t << endl;
    Allocated.insert(location, program);
    Loader(t, tree->head, startspace);
    return startspace;
  }
  return -1;
};

void Memory::Loader(string t, MemoryNode* node ,int s){
  int startspace = s + 5;
  int total;
  int mapping = 0;
  MemoryNode* aux = node;
  int branch = aux->branch;
  bool lastsegment = false;
  while (!lastsegment){
    total = aux->getMemory();
    int random = randomBinary();
    cout << "\033[35m" << "random on Loader: " << random << "\033[0m" << endl;
    pointers[startspace] = logicalMemory{t, startspace, total, aux->getID()};
    if (branch == 0){
      lastsegment = true;
    } else if (random == 1 || branch == 1){
      mapping += 1;
      mapping *= 10;
      aux->setNext(1);
      aux = aux->right;
    } else {
      mapping += 2;
      mapping *= 10;
      aux->setNext(2);
      aux = aux->left;
    }
    branch = aux->branch;
    startspace += total + 1;
  };
  mapping /= 10;
  pointers[s] = logicalMemory{t, s, 5, mapping};
};

void Memory::Reallocate(string t, MemoryTree* tree, int s){
  int mapping = pointers.find(s)->second.ID;
  int reverseMapping = invertInt(mapping);
  MemoryNode* node = tree->head;
  int location = s + 5 + node->getMemory() + 1;
  int newMapping = 0;

  while (node != tree->current){
    int digit = reverseMapping % 10;
    reverseMapping /= 10;
    if (digit){
      node = node->right;
    } else {
      node = node->left;
    };
    location += (node->getMemory() + 1);
    newMapping += digit;
    newMapping *= 10;
  };

  cout << "newMapping: " << newMapping << endl;
  if (node->getNext() == 1){
    node->setNext(2);
    node = node->left;
    newMapping += 2;
  } else {
    node->setNext(1);
    node = node->right;
    newMapping += 1;
  };
  newMapping *= 10;
  pointers[location] = logicalMemory{t, location, node->getMemory(), node->getID()};
  location += (node->getMemory() + 1);
  cout << "newMapping: " << newMapping << endl;
  while (node->branch != 0){
    if (randomBinary() == 1 || node->branch == 1){
      node->setNext(1);
      node = node->right;
      newMapping += 1;
    } else {
      node->setNext(2);
      node = node->left;
      newMapping += 2;
    }
    location += node->getMemory();
    pointers[location] = logicalMemory{t, location, node->getMemory(), node->getID()};
    newMapping *= 10;
  };
  newMapping /= 10;
  pointers[s] = logicalMemory{t, s, 5, newMapping};
};

int Memory::Unallocate(string t, MemoryTree* tree, bool k) {
  int start;
  cout << "unallocate type: " << t << ", tree node: " << tree->current->getID() << endl;
  for (int i = 0; i < Allocated.size(); i++) {
    if (Allocated[i].type == t) {
      if (k){
        start = Allocated[i].start;
        Allocated.erase(Allocated.begin() + i);
        Unload(t, tree->current, start);
        return -1;
      } else {
        MemoryNode* parent = tree->current;
        if (tree->current->getNext() == 1){
          tree->current = tree->current->right;
        } else {
          tree->current = tree->current->left;
        }
        Unload(t, tree->current, Allocated[i].start);
        tree->current = parent;
        return Allocated[i].start;
      }
    }
  }
}

void Memory::Unload(string t, MemoryNode* node, int s) {
  int mapping = pointers.find(s)->second.ID;
  int reverseMapping = invertInt(mapping);
  int index = s + 5; int keepSegment = 0; MemoryNode* aux = node;
  
  auto it = pointers.find(index);
  bool whileflag = true;
  cout << "reverseMapping 1: " << reverseMapping << endl;
  cout << "ID itvalue: " << it->second.ID << endl;
  cout << "ID aux: " << aux->getID() << endl;
  while (whileflag){
    cout << "Index: " << index << endl;
    int digit = reverseMapping % 10;
    cout << "reverseMapping 2: " << reverseMapping << endl;
    reverseMapping /= 10;
    cout << "reverseMapping 3: " << reverseMapping << endl;
    while (it->second.ID != aux->getID()){
      index += (it->second.total + 1);
      it = pointers.find(index);
      cout << "ID itvalue inside: " << it->second.ID << endl;
      digit = reverseMapping % 10;
      cout << "reverseMapping 4: " << reverseMapping << endl;
      reverseMapping /= 10;
      cout << "reverseMapping 5: " << reverseMapping << endl;
      keepSegment += 1;
      cout << "keepSegment: " << keepSegment << endl;
      cout << "Index insise while: " << index << endl;
    }

    pointers.erase(it);
    if (digit == 0){
      whileflag = false;
    } else {
      if (digit == 1){
        aux = aux->right;
      } else if (digit == 2){
        aux = aux->left;
      }
      cout << "ID aux inside: " << aux->getID() << endl;
      cout << "reverseMapping 6: " << reverseMapping << endl;
      index += (it->second.total + 1);
      it = pointers.find(index);
      if (reverseMapping == 0){
        pointers.erase(it);
        whileflag = false;
      }
    }
    
  }

  if (keepSegment != 0){
    cout << "\033[35m" << "MAPPING: " << mapping << endl;
    int numDigits = countDigits(mapping);
    cout << "NUMdigits: " << numDigits << endl;
    int newMapping = mapping / (pow(10,(numDigits - keepSegment + 1)));
    cout << "newMapping final: " << newMapping << "\033[0m" << endl;
    pointers[s] = logicalMemory{t, s, 5, newMapping};
  } else {
    it = pointers.find(s);
    pointers.erase(it);
  }
}

void Memory::Show(){
  cout << "\033[33m" << "Allocated ========" << endl;
  for (const logicalMemory lm : Allocated) {
    cout << "Type: " << lm.type << ", ID: " << lm.ID << ", Start: " << lm.start << ", Total: " << lm.total << endl;
  }
  cout << "Pointers ========" << endl;
  for (const auto lm : pointers){
    cout << "Type: " << lm.second.type << ", ID: " << lm.second.ID << ", Start: " << lm.second.start << ", Total: " << lm.second.total << endl;
  }
  cout << "========" << "\033[0m" << endl;
}