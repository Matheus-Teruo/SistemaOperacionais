#include <iostream>
#include "Event.h"
#include "LinkedList.h"
#include "Memory.h"
#include <string>
#include <list>
#include <queue>
#include <chrono>
#include <thread>
using namespace std;

// Simulation parametar
int typeOfMemory = 2; // 1 - simple e unique, 2 - First-fit, 3 Page
int typeOfProcess = 1; // 1 - unique, 2 - multi

// PC resources
int memory = 300;//Mb

LinkedList setup() {
  LinkedList eventlist;
  eventlist.insert(20, "Task1", 1, 30, 60);
  eventlist.insert(220, "Task3", 1, 80, 80);
  eventlist.insert(20, "Task2", 1, 100, 120);
  eventlist.insert(240, "Task4", 1, 40, 40); 

  return eventlist;
};



void eventEngine(LinkedList eventlist){
  // Variaveis do sistema
  bool simulation = true;
  int instant = 0;
  bool cpuUsage = false;
  int prev_termino;

  Event* event = nullptr;
  Event* eventFree;
  queue<Event*> waitQ;
  Memory memorySistem(memory);
  while (simulation) {
    instant++;
    cout << "instante: " << instant << endl;
    while (eventlist.head->getInstant() <= instant){
      event = eventlist.takeEvent();
      // cout << "Instante: " << event->getInstant() << ", Event: " << event->getType() << ", Flag: " << event->getFlag() << ", cpu_time: " << event->getcpuTime() << ", memory: " << event->getMemory() << endl;
      int flag = event->getFlag();
      switch(flag){
        case 1: // ingresso do job ao sistema // EXEÇÃO lista de espera dos jobs
          if (typeOfMemory == 1 && cpuUsage) {
            waitQ.push(event);
            cout << "entrada na lista de espera do job: " << event->getType() << endl;
            break;
          }
          eventlist.insert(0, event->getType(), 2, event->getMemory(), event->getcpuTime());
          cout << "ingresso ao sistema do job: " << event->getType() << endl;
          break;
        case 2: // Alocação de memória // EXCEÇÃO lista de espera por memória OU não disponivel de alocar
          cout << "alocacao de memoria do job: " << event->getType() << endl;
          if (typeOfMemory == 1){
            if (event->getMemory() > memory){
              cout << "nao ha memoria suficiente para alocar o job: " << event->getType() << endl;    
              break;
            }
          } else if (typeOfMemory == 2){
            int status = memorySistem.Loader(event->getType() ,event->getMemory());
            if (status == -2){
              cout << "nao ha memoria suficiente para alocar o job: " << event->getType() << endl;    
              break;
            } else if (status == -1){
              cout << "nao ha espaço contíguo disponivel em memoria fisica para o job: " << event->getType() << endl;    
              break;
            } else{
              cout << "job: " << event->getType() << ", alocado em: " << status << endl; 
            }
          } else if (typeOfMemory == 3){
            
          }
          eventlist.insert(0, event->getType(), 3, event->getMemory(), event->getcpuTime());
          break;
        case 3: // Alocação de processador // EXEÇÃO lista de espera por processador
          if (cpuUsage){
            waitQ.push(event);
            cout << "entrada na lista de espera por processador, do job: " << event->getType() << endl;
            break;
          }
          cpuUsage = true;
          prev_termino = instant + event->getcpuTime();
          eventlist.insert(prev_termino, event->getType(), 4, event->getMemory(), event->getcpuTime());
          cout << "alocacao de processador do job: " << event->getType() << endl;
          break;
        case 4: // Liberação de processador
          cout << "liberacao de processador do job: " << event->getType() << endl;
          if (typeOfMemory != 1){
            cpuUsage = false;
            if (!waitQ.empty()){
              eventFree = waitQ.front();
              waitQ.pop();
              eventlist.insert(0, eventFree->getType(), 3, eventFree->getMemory(), eventFree->getcpuTime());
            }
          }
          eventlist.insert(0, event->getType(), 5, event->getMemory(), event->getcpuTime());
          break;
        case 5: // Liberação de memória
          cout << "liberacao de memoria do job: " << event->getType() << endl;
          if (typeOfMemory == 2){
            bool statusUnload = memorySistem.Unload(event->getType(), event->getMemory());
            if (statusUnload){
              cout << "job: " << event->getType() << ", desalocado" << endl;
            } else if (!statusUnload){
              cout << "erro ao desalocar o job: " << event->getType() << endl;    
              break;
            }
          } else if (typeOfMemory == 3){
            
          }
          eventlist.insert(0, event->getType(), 6, event->getMemory(), event->getcpuTime());
          break;
        case 6: // Saída do sistema
          cout << "saida do sistema do job: " << event->getType() << endl;
          if (typeOfMemory == 1){
            cpuUsage = false;
            if(!waitQ.empty()){
              eventFree = waitQ.front();
              waitQ.pop();
              eventlist.insert(0, eventFree->getType(), 1, eventFree->getMemory(), eventFree->getcpuTime());
            }
          }
          break;
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