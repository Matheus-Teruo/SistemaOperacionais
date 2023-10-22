#include "EventNode.h"
#include "EventList.h"
#include "MemoryTree.h"
#include "Memory.h"
#include "RandomFunction.h"
#include <iostream>
#include <string>
#include <list>
#include <queue>
#include <chrono>
#include <thread>

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define RED     "\033[31m"     
#define GREEN   "\033[32m"    
#define YELLOW  "\033[33m"    
#define BLUE    "\033[34m"    
#define MAGENTA "\033[35m"    
#define CYAN    "\033[36m"    
#define WHITE   "\033[37m"

using namespace std;

// PC resources
int memory = 300;//Mb
int MaxmultiProg = 5;
int MaxwaitListCPU = 3; // ???
int timeSlice = 30;
int timeToChangeSegment = 5;
// fila de entrada e saída
// Uma fila única para o disco físico
// Arquivos diversas filas, cada uma para controlar o acesso a um arquivo específico em uso

// cout << "instante: " << event->getInstant() << ", tipo de evento: " << "ingresso do job no sistema" << ", id do programa: " << event->getType() << ", ação executada: " << event << ", resultados: " << << endl;

// Interrupções
MemoryTree* memoryIMemory = new MemoryTree({1,10});

EventList setup() {
  EventList eventlist;
  MemoryTree* memorytask1 = new MemoryTree({2,30,1,50});
  MemoryTree* memorytask2 = new MemoryTree({2,20,3,30,1,40,2,50,1,35});
  MemoryTree* memorytask3 = new MemoryTree({3,40,2,10,1,20,1,15});
  MemoryTree* memorytask4 = new MemoryTree({3,40,1,10,1,20});

  eventlist.create(20, "Task1", 1, memorytask1, (memorytask1->getMaxSegment()*timeSlice));
  eventlist.create(220, "Task3", 1, memorytask3, (memorytask3->getMaxSegment()*timeSlice - 10));
  eventlist.create(20, "Task2", 1, memorytask2, (memorytask2->getMaxSegment()*timeSlice));
  eventlist.create(240, "Task4", 1, memorytask4, (memorytask4->getMaxSegment()*timeSlice - 20)); 

  return eventlist;
};

void eventEngine(EventList eventlist){
  // Variaveis do sistema
  bool simulation = true;
  int instant = 0;
  bool cpuUsage = false;
  int prev_termino;
  int status;

  EventNode* event = nullptr;
  EventNode* eventFree;
  EventNode* eventHandled;
  queue<EventNode*> waitMemory;
  queue<EventNode*> waitProcessQ;
  queue<EventNode*> waitCPUQ;
  Memory memorySistem(memory);
  int auxTimeProcessing;
  while (simulation) {
    instant++;
    cout << RED << "instante: " << instant << RESET << endl;
    while (eventlist.head->getInstant() <= instant){
      event = eventlist.takeEvent();

      if (event->getStatus() != 2 && event->getStatus() != 3){
        cout << WHITE << BOLD << "instante: " << event->getInstant() 
        << ", " << event->getType()
        << ", cpu time: " << event->getcpuTime()
        << ", all memory: " << event->getMemoryTree()->getTotalMemory()
        << ", current memory:" << event->getMemoryActual() << RESET << endl;
      };
      
      // eventlist.display();
  
      int flag = event->getFlag();
      switch(flag){
        // ingresso do job ao sistema // EXEÇÃO lista de espera dos jobs
        case 1: 
          event->setInstant(0);event->setFlag(2);
          eventlist.insert(event);
          cout << GREEN << "ingresso ao sistema do job: " << event->getType() << RESET << endl;
          eventlist.display();
          break;
          
        // Alocação de memória // EXCEÇÃO lista de espera por memória OU não disponivel de alocar
        case 2: 
          cout << GREEN << "alocacao de memoria do job: " << event->getType() << RESET << endl;
          status = memorySistem.Allocate(event->getType() ,event->getMemoryTree());
          if (status == -2){  // Job maior que a memória
           cout << YELLOW << "nao ha memoria suficiente para alocar o job: " << event->getType() << RESET << endl;    
           waitMemory.push(event);
           break;
          } else if (status == -1){  // Não a espaço disponível
           cout << MAGENTA << "nao ha espaço contíguo disponivel em memoria fisica para o job: " << event->getType() << RESET << endl;    
           break;
          } else{  // Alocado dinheiro
           cout << GREEN << event->getType() << ", alocado no sistema em: " << status << RESET << endl; 
          };

          event->setInstant(0);event->setFlag(3);
          eventlist.insert(event);
          memorySistem.Show();
          break;

        // Alocação de processador // EXEÇÃO lista de espera por processador
        case 3:
          if (event->getStatus() == 0){  // 
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
              event->setStatus(1);
            };
          }
          if (event->getStatus() == 1){
            cpuUsage = true;
            auxTimeProcessing = event->getcpuTime();
            if(auxTimeProcessing > timeSlice){  // Se o programa ainda tem tempo maior que o time slice
              prev_termino = timeSlice + instant;
              if (event->getMemoryTree()->current->branch == 0){
                event->setInstant(prev_termino);event->setFlag(4);
                event->setcpuTime(auxTimeProcessing - timeSlice);
              } else if (randomBinary() == 1){  // Deixar levemente aleatório o momento de troca de segmento
                event->setInstant(prev_termino - timeToChangeSegment);event->setFlag(3);event->setStatus(2);  
                event->setcpuTime(auxTimeProcessing - (timeSlice - timeToChangeSegment));
              } else{
                event->setInstant(prev_termino - (timeToChangeSegment * 2));event->setFlag(3);event->setStatus(3);
                event->setcpuTime(auxTimeProcessing - (timeSlice - (timeToChangeSegment * 2)));
              }
            } else {  // Caso contrario ele zera
              prev_termino = instant + auxTimeProcessing;
              event->setInstant(prev_termino);event->setFlag(4);
              event->setcpuTime(0);
            };

            eventlist.insert(event);
            cout << GREEN << "alocacao de processador do job: " << event->getType() << RESET << endl;

          } else if (event->getStatus() == 2 || event->getStatus() == 3) {
            int randomInt = randomBinary();
            auxTimeProcessing = event->getcpuTime();
            // cout << MAGENTA << "numero de aleatorização de interrupção: " << randomInt << RESET << endl;
            if (randomInt == event->getMemoryTree()->current->getNext() || event->getMemoryTree()->current->branch == 1){  // Segmento ja carregado

              if (event->getStatus() == 2){
                event->setInstant(instant + timeToChangeSegment);
                event->setcpuTime(auxTimeProcessing - timeToChangeSegment);
              } else {
                event->setInstant(instant + (timeToChangeSegment*2));
                event->setcpuTime(auxTimeProcessing - (timeToChangeSegment*2));
              };
              event->getMemoryTree()->changeNode();
              event->setFlag(4); event->setStatus(1);
              eventlist.insert(event);

            } else {  // Ocorrencia de erro de segmento
              waitCPUQ.pop();
              eventHandled = event;
              waitCPUQ.push(event);  // Realoca a task no final da fila

              eventFree = new EventNode(0 ,"INTERRUPTION: MEMORY", 3, memoryIMemory, (memoryIMemory->getMaxSegment()*timeSlice - 25));
              eventFree->setStatus(4);  
              eventlist.insert(eventFree);
            }

          } else {  // INTERRUPÇÃO
            cout << YELLOW << " Realocate: " << eventHandled->getType() << RESET << endl;
            status = memorySistem.Unallocate(eventHandled->getType(), eventHandled->getMemoryTree(), false);
            memorySistem.Reallocate(eventHandled->getType(), eventHandled->getMemoryTree(), status);
            memorySistem.Show();
            eventFree = waitCPUQ.front();
            cout << GREEN << "preparando job " << eventFree->getType() << " para processamento" << RESET << endl;
            eventFree->setInstant(instant);eventFree->setFlag(3);eventHandled->setStatus(1);
            eventlist.insert(eventFree);
          }
          break;
        
        // Liberação de processador
        case 4:
          cout << GREEN << "liberacao de processador do job: " << event->getType() << RESET << endl;
          cpuUsage = false;

          if (event->getcpuTime() == 0){  // Verificar se o processo finalizou
            waitCPUQ.pop();  // Retira da lista
            if (!waitProcessQ.empty()){  // Verifica se a lista não está vazia
              eventFree = waitProcessQ.front();
              waitProcessQ.pop();
              eventFree->setInstant(0);eventFree->setFlag(3);
              eventlist.insert(eventFree);
            };
            event->setInstant(0);event->setFlag(5);  // Ajusta as flag para o finalizar a task
            eventlist.insert(event);
            
            if (!waitCPUQ.empty()){  // Verifica se a lista não está vazia
              eventFree = waitCPUQ.front();
              cout << GREEN << "preparando job " << eventFree->getType() << " para processamento" << RESET << endl;
              eventFree->setInstant(instant);eventFree->setFlag(3);
              eventlist.insert(eventFree);
            };
          } else {  // Caso o processo ainda não finalizou
            waitCPUQ.pop();
            cout << BLUE << "realocando job " << event->getType() << " ao final da fila" << RESET << endl;
            waitCPUQ.push(event);  // Realoca a task no final da fila
            
            eventFree = waitCPUQ.front();  // Retira um item para ser adicionado a lista
            cout << GREEN << "preparando job " << eventFree->getType() << " para processamento" << RESET << endl;
            eventFree->setInstant(instant);eventFree->setFlag(3);
            eventlist.insert(eventFree);
          };
          break;
        
        // Liberação de memória
        case 5:
          cout << GREEN << "liberacao de memoria do job: " << event->getType() << RESET << endl;
          event->getMemoryTree()->current = event->getMemoryTree()->head;
          status = memorySistem.Unallocate(event->getType(), event->getMemoryTree(), true);
          if (status){
            cout << BLUE << "job: " << event->getType() << ", desalocado" << RESET << endl;
            if (waitMemory.size() > 0){
              eventFree = waitMemory.front();
             cout << GREEN << "preparando job " << eventFree->getType() << " para alocação de memória" << RESET << endl;
              eventFree->setInstant(instant);eventFree->setFlag(2);
              eventlist.insert(eventFree);
            };
            memorySistem.Show();
          } else if (!status){
            cout << MAGENTA << "erro ao desalocar o job: " << event->getType() << RESET << endl;    
            break;
          };
          event->setInstant(0);event->setFlag(6);
          eventlist.insert(event);
          break;
        
        // Saída do sistema
        case 6:
          cout << GREEN << "saida do sistema do job: " << event->getType() << RESET << endl;
          eventlist.display();
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
  EventList eventlist = setup();
  eventlist.display();

  // Motor de eventos
  eventEngine(eventlist);
  return 0;
};