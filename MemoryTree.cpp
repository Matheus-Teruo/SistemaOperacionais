#include "MemoryTree.h"
#include "MemoryNode.h"
#include <list>
#include <iostream>

using namespace std;

MemoryTree::MemoryTree(list<int> t):
tree(t), totalMemory(0), maxMemoryOverlay(0), totalConditions(0),
useDevice1(0), useDevice2(0), auxID(0){
  it = tree.begin();
  int seg = 0; int MaxMOverlay = 0;
  int memo; int cpu; int disk; int device1; int device2;
  if(*it == 1){
    tree.erase(it++);memo = *it;
    tree.erase(it++);cpu = *it;
    tree.erase(it++);disk = *it;
    tree.erase(it++);device1 = *it; useDevice1 += device1;
    tree.erase(it++);device2 = *it; useDevice2 += device2;
    MemoryNode* no = new MemoryNode(auxID++, memo, cpu, disk, device1, device2);
    totalMemory += memo; maxMemoryOverlay += memo;
    current = head = no;
  }
  
  else if(*it == 2){
    tree.erase(it++);memo = *it;
    tree.erase(it++);cpu = *it;
    tree.erase(it++);disk = *it;
    tree.erase(it++);device1 = *it; useDevice1 += device1;
    tree.erase(it++);device2 = *it; useDevice2 += device2;
    MemoryNode* no = new MemoryNode(auxID++, memo, cpu, disk, device1, device2);
    totalMemory += memo; MaxMOverlay += memo;
    current = head = no;
    
    tree.erase(it++);

    AuxFunc(no, ++seg, MaxMOverlay);
  }
  
  else if(*it == 3){
    totalConditions++;
    tree.erase(it++);memo = *it;
    tree.erase(it++);cpu = *it;
    tree.erase(it++);disk = *it;
    tree.erase(it++);device1 = *it; useDevice2 += device2;
    tree.erase(it++);device2 = *it; useDevice2 += device2;
    MemoryNode* no = new MemoryNode(auxID++, memo, cpu, disk, device1, device2);
    totalMemory += memo; MaxMOverlay += memo;
    current = head = no;
    
    tree.erase(it++);
    AuxFunc(no, ++seg, MaxMOverlay);
    tree.erase(it++);
    AuxFunc(no, seg, MaxMOverlay);
  }
};

void MemoryTree::AuxFunc(MemoryNode* parent, int segment, int MaxMOverlay){
  int auxSeg;
  int memo; int cpu; int disk; int device1; int device2;
  if(*it == 1){
    tree.erase(it++);memo = *it;
    tree.erase(it++);cpu = *it;
    tree.erase(it++);disk = *it;
    tree.erase(it++);device1 = *it; useDevice1 += device1;
    tree.erase(it++);device2 = *it; useDevice2 += device2;
    MemoryNode* no = new MemoryNode(auxID++, memo, cpu, disk, device1, device2);
    totalMemory += memo; MaxMOverlay += memo;
    auxSeg = segment + 1;
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
    tree.erase(it++);memo = *it;
    tree.erase(it++);cpu = *it;
    tree.erase(it++);disk = *it;
    tree.erase(it++);device1 = *it; useDevice1 += device1;
    tree.erase(it++);device2 = *it; useDevice2 += device2;
    MemoryNode* no = new MemoryNode(auxID++, memo, cpu, disk, device1, device2);
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
    tree.erase(it++);memo = *it;
    tree.erase(it++);cpu = *it;
    tree.erase(it++);disk = *it;
    tree.erase(it++);device1 = *it; useDevice1 += device1;
    tree.erase(it++);device2 = *it; useDevice2 += device2;
    MemoryNode* no = new MemoryNode(auxID++, memo, cpu, disk, device1, device2);
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

int MemoryTree::getUseDevice1() const{
  return useDevice1;
}

int MemoryTree::getUseDevice2() const{
  return useDevice2;
}

void MemoryTree::changeNode() {
  if(current->getNext() == 1){
    current = current->right;
  } else {
    current = current->left;
  }
}

MemoryNode::MemoryNode(int i, int m, int t, int d, int io1, int io2):
ID(i), memory(m), cpuT(t), disk(d), device1(io1), device2(io2), next(0), right(nullptr), left(nullptr), branch(0){};

int MemoryNode::getID() const{
  return ID;
};

int MemoryNode::getMemory() const{
  return memory;
};

int MemoryNode::getcpuT() const{
  return cpuT;
}

int MemoryNode::getdisk() const{
  return disk;
}

int MemoryNode::getdevice1() const{
  return device1;
}

int MemoryNode::getdevice2() const{
  return device2;
}

int MemoryNode::getNext() const{
  return next;
}

void MemoryNode::setdevice1(int s){
  device1 = s;
}

void MemoryNode::setdevice2(int s){
  device2 = s;
}

void MemoryNode::setNext(int n){
  next = n;
}