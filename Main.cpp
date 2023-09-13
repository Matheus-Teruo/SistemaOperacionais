#include <iostream>
#include "Event.h"
#include "LinkedList.h"

using namespace std;

LinkedList setup() {
  LinkedList eventlist;
  eventlist.insert(20, "Iniciar", 30, 60);
  eventlist.insert(220, "Task2", 80, 80);
  eventlist.insert(20, "Task1", 100, 120);
  eventlist.insert(240, "Finalizar", 40, 40); 

  return eventlist;
};

int main() {
  LinkedList eventlist = setup();

  eventlist.display();
  
  return 0;
};