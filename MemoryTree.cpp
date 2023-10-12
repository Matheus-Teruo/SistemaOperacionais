#include "MemoryTree.h"
#include "MemoryNode.h"
#include <list>
#include <iostream>

using namespace std;

MemoryTree::MemoryTree(list<int> t): tree(t), maxSegment(0), totalMemory(0), totalConditions(0){
  it = tree.begin();
  int seg = 0;
  if(*it == 1){
    //cout << "code " << 1 << endl;
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    //cout << "memory add " << *it << endl;
    totalMemory += *it;
    maxSegment++;
    head = &no;
  }else if(*it == 2){
    //cout << "code " << 2 << endl;
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    //cout << "memory add " << *it << endl;
    totalMemory += *it;
    seg++;
    head = &no;
    tree.erase(it++);
    AuxFunc(&no, seg);
  }else if(*it == 3){
    //cout << "code " << 3 << endl;
    totalConditions++;
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    //cout << "memory add " << *it << endl;
    totalMemory += *it;
    seg++;
    head = &no;
    tree.erase(it++);
    AuxFunc(&no, seg);
    tree.erase(it++);
    AuxFunc(&no, seg);
  }
};

void MemoryTree::AuxFunc(MemoryNode* head, int segment){
  int auxSeg;
  if(*it == 1){
    //cout << "code " << 1 << endl;
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    //cout << "memory add " << *it << endl;
    totalMemory += *it;
    auxSeg = segment + 1;
    //cout << "endpoint, numero segmento" << auxSeg << endl;
    if (maxSegment < auxSeg){
      maxSegment = auxSeg;
    };
    if (head->right != nullptr){
      head->right = &no;
    }else{
      head->left = &no;
    }
  }else if(*it == 2){
    //cout << "code " << 2 << endl;
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    //cout << "memory add " << *it << endl;
    totalMemory += *it;
    tree.erase(it++);
    if (head->right != nullptr){
      head->right = &no;
    }else{
      head->left = &no;
    }
    auxSeg = segment + 1;
    AuxFunc(&no, auxSeg);
  }else if(*it == 3){
    //cout << "code " << 3 << endl;
    totalConditions++;
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    //cout << "memory add " << *it << endl;
    totalMemory += *it;
    tree.erase(it++);
    if (head->right != nullptr){
      head->right = &no;
    }else{
      head->left = &no;
    }
    auxSeg = segment + 1;
    AuxFunc(&no, auxSeg);
    tree.erase(it++);
    AuxFunc(&no, auxSeg);
  }
  return;
};

int MemoryTree::getTotalMemory() const{
  return totalMemory;
};

int MemoryTree::getTotalConditional() const{
  return totalConditions;
};

int MemoryTree::getMaxSegment() const{
  return maxSegment;
};

MemoryNode::MemoryNode(int m): memory(m), right(nullptr), left(nullptr){};

int MemoryNode::getMemory() const{
  return memory;
};