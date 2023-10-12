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
int MaxmultiProg = 5;
int MaxwaitListCPU = 3; // ???
int timeSlice = 30;
// fila de entrada e saída
// Uma fila única para o disco físico
// Arquivos diversas filas, cada uma para controlar o acesso a um arquivo específico em uso

// cout << "instante: " << event->getInstant() << ", tipo de evento: " << "ingresso do job no sistema" << ", id do programa: " << event->getType() << ", ação executada: " << event << ", resultados: " << << endl;

LinkedList setup() {
  LinkedList eventlist;
  MemoryTree* memorytask1 = new MemoryTree({2,30,1,50});
  MemoryTree* memorytask2 = new MemoryTree({2,20,3,30,1,40,2,50,1,35});
  MemoryTree memorytask3 = MemoryTree({3,40,2,10,1,20,1,15});
  MemoryTree memorytask4 = MemoryTree({3,40,1,10,1,20});

  cout << memorytask1->getTotalMemory() << endl;
  cout << memorytask2->getTotalMemory() << endl;
  cout << memorytask3.getTotalMemory() << endl;
  cout << memorytask4.getTotalMemory() << endl;

  eventlist.create(20, "Task1", 1, memorytask1, (memorytask1->getMaxSegment()*timeSlice));
  eventlist.create(220, "Task3", 1, &memorytask3, (memorytask3.getMaxSegment()*timeSlice - 10));
  eventlist.create(20, "Task2", 1, memorytask2, (memorytask2->getMaxSegment()*timeSlice));
  eventlist.create(240, "Task4", 1, &memorytask4, (memorytask4.getMaxSegment()*timeSlice - 20)); 

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
  queue<Event*> waitProcessQ;
  queue<Event*> waitCPUQ;
  Memory memorySistem(memory);
  int auxTimeProcessing;
  while (simulation) {
    instant++;
    cout << "instante: " << instant << endl;
    while (eventlist.head->getInstant() <= instant){
      event = eventlist.takeEvent();
      cout << "Instante: " << event->getInstant() << ", event: " << event->getType() << ", flag: " << event->getFlag() << ", cpu_time: " << event->getcpuTime() << ", all memory: " << event->getTotalMemory() << endl;
      eventlist.display();
      int flag = event->getFlag();
      switch(flag){
        // ingresso do job ao sistema // EXEÇÃO lista de espera dos jobs
        case 1: 
          event->setInstant(0);
          event->setFlag(2);
          eventlist.insert(event);
          cout << "ingresso ao sistema do job: " << event->getType() << endl;
          break;
          
        // Alocação de memória // EXCEÇÃO lista de espera por memória OU não disponivel de alocar
        case 2: 
          cout << "alocacao de memoria do job: " << event->getType() << endl;
          //int status = memorySistem.Loader(event->getType() ,event->getMemory());
          //if (status == -2){
          //  cout << "nao ha memoria suficiente para alocar o job: " << event->getType() << endl;    
          //  break;
          //} else if (status == -1){
          //  cout << "nao ha espaço contíguo disponivel em memoria fisica para o job: " << event->getType() << endl;    
          //  break;
          //} else{
          //  cout << "job: " << event->getType() << ", alocado em: " << status << endl; 
          //}
          event->setInstant(0);
          event->setFlag(3);
          eventlist.insert(event);
          break;

        // Alocação de processador // EXEÇÃO lista de espera por processador
        case 3:
          if (event->getStatus() == 0){
            cout << "puxando da lista" << endl;
            if (waitCPUQ.size() >= MaxwaitListCPU){
              waitProcessQ.push(event);
              break;
            } else {
              event->setStatus(1);
              if (waitCPUQ.size() > 0){
                waitCPUQ.push(event);
                break;  
              }
              waitCPUQ.push(event);
            };
          }
          cpuUsage = true;
          auxTimeProcessing = event->getcpuTime();
          cout << auxTimeProcessing << endl;
          if(auxTimeProcessing > timeSlice){
            prev_termino = timeSlice + instant;
            event->setcpuTime(auxTimeProcessing - timeSlice);
          } else {
            prev_termino = instant + auxTimeProcessing;
            event->setcpuTime(0);
          }
          event->setInstant(prev_termino);
          event->setFlag(4);
          eventlist.insert(event);
          cout << "alocacao de processador do job: " << event->getType() << endl;
          break;
        
        // Liberação de processador
        case 4:
          cout << "liberacao de processador do job: " << event->getType() << endl;
          cpuUsage = false;
          if (event->getcpuTime() == 0){
            waitCPUQ.pop();
            if (!waitProcessQ.empty()){
              eventFree = waitProcessQ.front();
              waitProcessQ.pop();
              eventFree->setInstant(0);
              eventFree->setFlag(3);
              eventlist.insert(eventFree);
            }
            event->setInstant(0);
            event->setFlag(5);
            eventlist.insert(event);
            
            if (waitCPUQ.size() > 0){
              eventFree = waitCPUQ.front();
              cout << "preparando job " << eventFree->getType() << " para processamento" << endl;
              eventFree->setInstant(instant);
              eventFree->setFlag(3);
              eventlist.insert(eventFree);  
            }
          } else {
            waitCPUQ.pop();
            cout << "realocando job " << event->getType() << " ao final da fila" << endl;
            waitCPUQ.push(event);
            
            eventFree = waitCPUQ.front();
            cout << "preparando job " << eventFree->getType() << " para processamento" << endl;
            eventFree->setInstant(instant);
            eventFree->setFlag(3);
            eventlist.insert(eventFree);
            break;
          }
          break;
        
        // Liberação de memória
        case 5:
          cout << "liberacao de memoria do job: " << event->getType() << endl;
          //bool statusUnload = memorySistem.Unload(event->getType(), event->getMemory());
          //if (statusUnload){
          //  cout << "job: " << event->getType() << ", desalocado" << endl;
          //} else if (!statusUnload){
          //  cout << "erro ao desalocar o job: " << event->getType() << endl;    
          //  break;
          //}
          event->setInstant(0);
          event->setFlag(6);
          eventlist.insert(event);
          break;
        
        // Saída do sistema
        case 6:
          cout << "saida do sistema do job: " << event->getType() << endl;
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