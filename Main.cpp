#include <iostream>
#include "Event.h"
#include "LinkedList.h"
#include <string>
#include <list>
#include <queue>
#include <chrono>
#include <thread>
using namespace std;

LinkedList setup() {
  LinkedList eventlist;
  eventlist.insert(20, "Iniciar", 1, 30, 60);
  eventlist.insert(220, "Task2", 1, 80, 80);
  eventlist.insert(20, "Task1", 1, 100, 120);
  eventlist.insert(240, "Finalizar", 1, 40, 40); 

  return eventlist;
};

void eventEngine(LinkedList eventlist){
  // Variaveis do sistema
  bool simulation = true;
  int instant = 0;
  bool cpuUsage = false;
  int prev_termino;

  Event* event = nullptr;
  queue<Event*> waitQ;
  while (simulation) {
    instant++;
    // cout << "instant: " << instant << endl;
    while (eventlist.head->getInstant() <= instant){
      event = eventlist.takeEvent();
      cout << "Instant: " << event->getInstant() << ", Event: " << event->getType() << ", Flag: " << event->getFlag() << ", cpu_time: " << event->getcpuTime() << ", memory: " << event->getMemory() << endl;
      int flag = event->getFlag();
      switch(flag){
        case 1:
          if (cpuUsage) {
            waitQ.push(event);
            // lista de espera dos jobs
            cout << "entrada na lista de espera do job: " << event->getType() << endl;
            break;
          }
          eventlist.insert(0, event->getType(), 2, event->getMemory(), event->getcpuTime());
          // ingresso do job ao sistema
          cout << "ingresso ao sistema do job: " << event->getType() << endl;
          break;
        case 2:
          eventlist.insert(0, event->getType(), 3, event->getMemory(), event->getcpuTime());
          // alocação de memória
          // PROGRAMAR SISTEMA DE MEMÓRIA
          cout << "alocacao de memoria do job: " << event->getType() << endl;
          break;
        case 3:
          prev_termino = instant + event->getcpuTime();
          cpuUsage = true;
          eventlist.insert(prev_termino, event->getType(), 4, event->getMemory(), event->getcpuTime());
          // alocação de processador
          cout << "alocacao de processador do job: " << event->getType() << endl;
          break;
        case 4:
          eventlist.insert(0, event->getType(), 5, event->getMemory(), event->getcpuTime());
          cpuUsage = false;
          // liberação de processador
          cout << "liberacao de processador do job: " << event->getType() << endl;
          break;
        case 5:
          eventlist.insert(0, event->getType(), 6, event->getMemory(), event->getcpuTime());
          // liberação de memória
          cout << "liberacao de memoria do job: " << event->getType() << endl;
          break;
        case 6:
          // saida do sistema
          cout << "saida do sistema do job: " << event->getType() << endl;
          if (!waitQ.empty()){
            event = waitQ.front();
            // cout << waitQ.front()->getType() << endl;
            waitQ.pop();
            eventlist.insert(0, event->getType(), 1, event->getMemory(), event->getcpuTime());
          }
      }
    }
    if (eventlist.head == nullptr){
      simulation = false;
    }
    // this_thread::sleep_for(chrono::milliseconds(100));
  }
}



int main() {
  LinkedList eventlist = setup();
  eventlist.display();

  // Motor de eventos
  eventEngine(eventlist);
  return 0;
};