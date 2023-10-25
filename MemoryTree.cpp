#include "MemoryTree.h"
#include "MemoryNode.h"
#include <list>
#include <iostream>

using namespace std;

MemoryTree::MemoryTree(list<int> t): tree(t), maxSegment(0), totalMemory(0), maxMemoryOverlay(0), totalConditions(0), auxID(0){
  it = tree.begin();
  int seg = 0; int MaxMOverlay = 0;
  int memo;
  if(*it == 1){
    tree.erase(it++);
    memo = *it;
    tree.erase(it++);
    MemoryNode* no = new MemoryNode(auxID++, memo, *it);
    totalMemory += memo; maxMemoryOverlay += memo;
    maxSegment++; 
    current = head = no;
  }
  
  else if(*it == 2){
    tree.erase(it++);
    memo = *it;
    tree.erase(it++);
    MemoryNode* no = new MemoryNode(auxID++, memo, *it);
    totalMemory += memo; MaxMOverlay += memo;
    current = head = no;
    
    tree.erase(it++);

    AuxFunc(no, ++seg, MaxMOverlay);
  }
  
  else if(*it == 3){
    totalConditions++;
    tree.erase(it++);
    memo = *it;
    tree.erase(it++);
    MemoryNode* no = new MemoryNode(auxID++, memo, *it);
    totalMemory += memo; MaxMOverlay += memo;
    current = head = no;
    
    tree.erase(it++);
    AuxFunc(no, ++seg, MaxMOverlay);
    tree.erase(it++);
    AuxFunc(no, seg, MaxMOverlay);
  }
  delete &it;
  delete &tree;
};

void MemoryTree::AuxFunc(MemoryNode* parent, int segment, int MaxMOverlay){
  int auxSeg;
  int memo;
  if(*it == 1){
    tree.erase(it++);
    memo = *it;
    tree.erase(it++);
    MemoryNode* no = new MemoryNode(auxID++, memo, *it);
    totalMemory += memo; MaxMOverlay += memo;
    auxSeg = segment + 1;
    if (maxSegment < auxSeg){maxSegment = auxSeg;};
    if (maxMemoryOverlay < MaxMOverlay){maxMemoryOverlay = MaxMOverlay;};
    if (parent->branch == 0){
      parent->right = no;
      parent->branch = 1;
    }else{
      parent->left = no;
      parent->branch = 2;
    }
  }
  
  else if(*it == 2){
    tree.erase(it++);
    memo = *it;
    tree.erase(it++);
    MemoryNode* no = new MemoryNode(auxID++, memo, *it);
    totalMemory += memo; MaxMOverlay += memo;
    auxSeg = segment + 1;
    tree.erase(it++);
    
    if (parent->branch == 0){
      parent->right = no;
      parent->branch = 1;
    }else{
      parent->left = no;
      parent->branch = 2;
    }
    AuxFunc(no, auxSeg, MaxMOverlay);
  }
  
  else if(*it == 3){
    totalConditions++;
    tree.erase(it++);
    memo = *it;
    tree.erase(it++);
    MemoryNode* no = new MemoryNode(auxID++, memo,*it);
    totalMemory += memo;  MaxMOverlay += memo;
    auxSeg = segment + 1;
    tree.erase(it++);

    if (parent->branch == 0){
      parent->right = no;
      parent->branch = 1;
    }else{
      parent->left = no;
      parent->branch = 2;
    }
    AuxFunc(no, auxSeg, MaxMOverlay);
    tree.erase(it++);
    AuxFunc(no, auxSeg, MaxMOverlay);
  }
  return;
};

int MemoryTree::getTotalMemory() const{
  return totalMemory;
};

int MemoryTree::getMaxMemoryOverlay() const{
  return maxMemoryOverlay;
};

int MemoryTree::getTotalConditional() const{
  return totalConditions;
};

int MemoryTree::getMaxSegment() const{
  return maxSegment;
};

void MemoryTree::changeNode() {
  if(current->getNext() == 1){
    current = current->right;
  } else {
    current = current->left;
  }
}

MemoryNode::MemoryNode(int i, int m, int t): ID(i), memory(m), CPUTimeSeg(t), next(0), right(nullptr), left(nullptr), branch(0){};

int MemoryNode::getID() const{
  return ID;
};

int MemoryNode::getMemory() const{
  return memory;
};

int MemoryNode::getcpuTimeSeg() const{
  return CPUTimeSeg;
}

int MemoryNode::getNext() const{
  return next;
}

void MemoryNode::setNext(int n){
  next = n;
}