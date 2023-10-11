#include "MemoryTree.h"
#include "MemoryNode.h"
#include <list>

using namespace std;

MemoryTree::MemoryTree(list<int> tree){
  auto it = tree.begin();
  int seg;
  if(*it == 1){
    tree.erase(it++); 
    MemoryNode no = MemoryNode(*it);
    totalMemory += *it;
    maxSegment++;
    head = &no;
  }else if(*it == 2){
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    totalMemory += *it;
    seg++;
    head = &no;
    tree.erase(it++);
    AuxFunc(tree, &no, seg);
  }else if(*it == 3){
    totalConditions++;
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    totalMemory += *it;
    seg++;
    head = &no;
    tree.erase(it++);
    AuxFunc(tree, &no, seg);
    AuxFunc(tree, &no, seg);
  }
};

void MemoryTree::AuxFunc(list<int> tree, MemoryNode* head, int segment){
  auto it = tree.begin();
  int auxSeg;
  if(*it == 1){
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    totalMemory += *it;
    if (maxSegment < segment){
      maxSegment = segment;
    };
    if (head->right != nullptr){
      head->right = &no;
    }else{
      head->left = &no;
    }
  }else if(*it == 2){
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    totalMemory += *it;
    tree.erase(it++);
    if (head->right != nullptr){
      head->right = &no;
    }else{
      head->left = &no;
    }
    auxSeg = segment + 1;
    AuxFunc(tree, &no, auxSeg);
  }else if(*it == 3){
    totalConditions++;
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    totalMemory += *it;
    tree.erase(it++);
    if (head->right != nullptr){
      head->right = &no;
    }else{
      head->left = &no;
    }
    auxSeg = segment + 1;
    AuxFunc(tree, &no, auxSeg);
    AuxFunc(tree, &no, auxSeg);
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