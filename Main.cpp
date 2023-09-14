#include <iostream>
#include "Event.h"
#include "LinkedList.h"
#include <string>
#include <list>
#include <chrono>
#include <thread>
using namespace std;

LinkedList setup() {
  LinkedList eventlist;
  eventlist.insert(20, "Iniciar", 30, 60);
  eventlist.insert(220, "Task2", 80, 80);
  eventlist.insert(20, "Task1", 100, 120);
  eventlist.insert(240, "Finalizar", 40, 40); 

  return eventlist;
};

void eventEngine(LinkedList eventlist){
  bool simulation = true;
  int instant = 0;
  while (simulation) {
    instant++;
    cout << "instant: " << instant << endl;
    while (eventlist.head->getInstant() == instant){
      Event* event = eventlist.takeEvent();
      //this_thread::sleep_for(chrono::milliseconds(200));
      cout << "Instant: " << event->getInstant() << ", Event: " << event->getType() << ", cpu_time: " << event->getcpuTime() << ", memory: " << event->getMemory() << endl;
    }
    if (eventlist.head == nullptr){
      simulation = false;
    }
  }
}



int main() {
  LinkedList eventlist = setup();
  eventlist.display();

  // Motor de eventos
  eventEngine(eventlist);
  return 0;
};