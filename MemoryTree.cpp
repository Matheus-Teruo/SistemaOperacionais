#include "MemoryTree.h"
#include <list>

using namespace std;

MemoryTree::MemoryTree(list<int> tree){
  auto it = tree.begin();
  if(*it == 1){
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    head = &no;
  }else if(*it == 2){
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    head = &no;
    tree.erase(it++);
    AuxFunc(tree, &no);
  }else if(*it == 3){
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    head = &no;
    tree.erase(it++);
    AuxFunc(tree, &no);
    AuxFunc(tree, &no);
  }
};

void MemoryTree::AuxFunc(list<int> tree, MemoryNode* head){
  auto it = tree.begin();
  if(*it == 1){
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    if (head->right != nullptr){
      head->right = &no;
    }else{
      head->left = &no;
    }
  }else if(*it == 2){
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    tree.erase(it++);
    if (head->right != nullptr){
      head->right = &no;
    }else{
      head->left = &no;
    }
    AuxFunc(tree, &no);
  }else if(*it == 3){
    tree.erase(it++);
    MemoryNode no = MemoryNode(*it);
    tree.erase(it++);
    if (head->right != nullptr){
      head->right = &no;
    }else{
      head->left = &no;
    }
    AuxFunc(tree, &no);
    AuxFunc(tree, &no);
  }
  return;
};

int MemoryTree::TotalMemory() const{
  return totalmemory;
};

int MemoryTree::TotalConditional() const{
  return totalconditions;
};

MemoryNode::MemoryNode(int m): memory(m), right(nullptr), left(nullptr){};

int MemoryNode::getMemory() const{
  return memory;
};