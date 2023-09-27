#include <iostream>
#include "Event.h"
#include "MemoryTree.h"
#include "LinkedList.h"
#include "Memory.h"
#include <string>
#include <list>
#include <queue>
#include <chrono>
#include <thread>
using namespace std;

// PC resources
int memory = 300;//Mb
int MaxmultiProg = 3;
int MaxwaitListCPU = 5; // ???
// fila de entrada e saída
// Uma fila única para o disco físico
// Arquivos diversas filas, cada uma para controlar o acesso a um arquivo específico em uso

// cout << "instante: " << event->getInstant() << ", tipo de evento: " << "ingresso do job no sistema" << ", id do programa: " << event->getType() << ", ação executada: " << event << ", resultados: " << << endl;

LinkedList setup() {
  LinkedList eventlist;
  MemoryTree memorytask1 = MemoryTree({1,30});
  MemoryTree memorytask2 = MemoryTree({2,20,3,30,1,40,1,50});
  MemoryTree memorytask3 = MemoryTree({3,40,2,10,1,20,1,15});
  MemoryTree memorytask4 = MemoryTree({3,40,1,10,2,20,1,15});

  eventlist.insert(20, "Task1", 1, &memorytask1, 60);
  eventlist.insert(220, "Task3", 1, &memorytask2, 80);
  eventlist.insert(20, "Task2", 1, &memorytask3, 120);
  eventlist.insert(240, "Task4", 1, &memorytask4, 40); 

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
          eventlist.insert(0, event->getType(), 2, event->getMemoryTree(), event->getcpuTime());
          cout << "ingresso ao sistema do job: " << event->getType() << endl;
          break;
        case 2: // Alocação de memória // EXCEÇÃO lista de espera por memória OU não disponivel de alocar
          cout << "alocacao de memoria do job: " << event->getType() << endl;
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
          eventlist.insert(0, event->getType(), 3, event->getMemoryTree(), event->getcpuTime());
          break;
        case 3: // Alocação de processador // EXEÇÃO lista de espera por processador
          if (cpuUsage){
            waitQ.push(event);
            cout << "entrada na lista de espera por processador, do job: " << event->getType() << endl;
            break;
          }
          cpuUsage = true;
          prev_termino = instant + event->getcpuTime();
          eventlist.insert(prev_termino, event->getType(), 4, event->getMemoryTree(), event->getcpuTime());
          cout << "alocacao de processador do job: " << event->getType() << endl;
          break;
        case 4: // Liberação de processador
          cout << "liberacao de processador do job: " << event->getType() << endl;
          cpuUsage = false;
          if (!waitQ.empty()){
            eventFree = waitQ.front();
            waitQ.pop();
            eventlist.insert(0, eventFree->getType(), 3, eventFree->getMemoryTree(), eventFree->getcpuTime());
          }
          eventlist.insert(0, event->getType(), 5, event->getMemoryTree(), event->getcpuTime());
          break;
        case 5: // Liberação de memória
          cout << "liberacao de memoria do job: " << event->getType() << endl;
          bool statusUnload = memorySistem.Unload(event->getType(), event->getMemory());
          if (statusUnload){
            cout << "job: " << event->getType() << ", desalocado" << endl;
          } else if (!statusUnload){
            cout << "erro ao desalocar o job: " << event->getType() << endl;    
            break;
          }
          eventlist.insert(0, event->getType(), 6, event->getMemoryTree(), event->getcpuTime());
          break;
        case 6: // Saída do sistema
          cout << "saida do sistema do job: " << event->getType() << endl;
          cpuUsage = false;
          if(!waitQ.empty()){
            eventFree = waitQ.front();
            waitQ.pop();
            eventlist.insert(0, eventFree->getType(), 1, eventFree->getMemoryTree(), eventFree->getcpuTime());
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