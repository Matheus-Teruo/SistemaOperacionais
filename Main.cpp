#include "EventNode.h"
#include "EventList.h"
#include "MemoryTree.h"
#include "Memory.h"
#include "RandomFunction.h"
#include <iostream>
#include <string>
#include <list>
#include <queue>
#include <map>

#define RESET   "\033[0m"
#define BOLD    "\033[1m"
#define UNBOLD  "\033[22m" 
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
int timeBeforeChangeSeg = 5;
int timeToMemoryProcess = 5;
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
  
  // Variaveis para evento
  EventNode* event = nullptr;
  EventNode* eventFree;
  EventNode* eventHandled;
  // Filas de espera
  queue<EventNode*> waitMemory;

  queue<EventNode*> waitProcessQ;

  map<string,EventNode*> tasksBlocked;
  queue<EventNode*> waitCPUQ;

  // Componentes do sistema
  Memory memorySistem(memory);
  
  // Variaveis auxiliar do sistema
  bool simulation = true;
  int instant = 0;
  int prev_termino;
  int status;
  int auxTimeProcessing;
  bool cpuUsage = false; 

  while (simulation) {
    instant++;
    cout << RED << "instante: " << instant << RESET << endl;
    while (eventlist.head->getInstant() <= instant){
      event = eventlist.takeEvent();

      // Print de retirada da lista
      if (event->getStatus() != 2 && event->getStatus() != 3){  // Gambiarra para ajustar algumas interrupções
        cout << WHITE << BOLD << "instante: " << event->getInstant() 
        << ", " << event->getType()
        << ", cpu time: " << event->getcpuTime()
        << ", current memory ID: " << event->getMemoryTree()->current->getID()
        << ", current memory:" << event->getMemoryActual() << RESET << endl;
      };
      
      eventlist.display();  // mostrar eventos ainda dentro da lista
  
      int flag = event->getFlag();
      switch(flag){
        // INGRESSO DO JOB AO SISTEMA
        case 1: 
          cout << GREEN << "ingresso ao sistema do job: " << BOLD << event->getType() << RESET << endl;
          eventlist.display();
          event->setInstant(0);event->setFlag(2);
          eventlist.insert(event);
          break;
          
        // ALOCAÇÃO DE MEMÓRIA
        case 2:
          if (event->getStatus() == 0){  // Allocação inicial do programa
            cout << GREEN << "alocacao de memoria do job: " << BOLD << event->getType() << RESET << endl;
            status = memorySistem.Allocate(event->getType() ,event->getMemoryTree());
            if (status == -2){  // Job maior que a memória
              cout << YELLOW << "nao ha memoria suficiente para alocar o job: " << BOLD << event->getType() << RESET << endl;    
              waitMemory.push(event);
              break;
            } else if (status == -1){  // Não a espaço disponível
              cout << MAGENTA << "nao ha espaço contiguo disponivel em memoria fisica para o job: " << BOLD << event->getType() << RESET << endl;    
              break;
            } else{  // Alocado dinheiro
              cout << GREEN << event->getType() << ", alocado no sistema em: " << status << RESET << endl; 
            };
            event->setInstant(instant + timeToMemoryProcess);event->setFlag(3);
            eventlist.insert(event);

          } else if (event->getStatus() == 1){  // Realocação do program
            cout << YELLOW << " Realocate: " << BOLD << event->getType() << RESET << endl;
            status = memorySistem.Unallocate(event->getType(), event->getMemoryTree(), false);
            memorySistem.Reallocate(event->getType(), event->getMemoryTree(), status);
            event->getMemoryTree()->changeNode();
            auto it = tasksBlocked.find(event->getType());
            if (it != tasksBlocked.end()){
              tasksBlocked.erase(it);
            };
            if (waitCPUQ.empty()){
              event->setInstant(instant + timeToMemoryProcess);event->setFlag(3);
              eventlist.insert(event);
            }
            waitCPUQ.push(event);
            eventlist.display();
          }
          memorySistem.Show();
          break;

        // ALOCAÇÃO DE PROCESSADOR OU FILA DE PROCESSAMENTO
        case 3:
          if (event->getStatus() == 0){  // PRIMEIRA ENTRADA DA TASK
            if ((waitCPUQ.size() + tasksBlocked.size()) >= MaxwaitListCPU){  // Número máximo de job processando
              waitProcessQ.push(event);
              break;
            } else {
              event->setStatus(1);
              if ((waitCPUQ.size()) > 0){  // Lista já tem item á frente
                cout << GREEN << "Posicionando: " << BOLD << event->getType() << UNBOLD << " na fila de espera" << RESET << endl;
                waitCPUQ.push(event);
                break;  
              }
              waitCPUQ.push(event);
            };
          }
          if (event->getStatus() == 1){  // Começo das task
            auxTimeProcessing = event->getcpuTime();
            cpuUsage = true;
            if(auxTimeProcessing > timeSlice){  // Se o programa ainda tem tempo maior que o time slice
              prev_termino = timeSlice + instant;
              if (event->getMemoryTree()->current->branch == 0){
                event->setInstant(prev_termino);event->setFlag(4);
                event->setcpuTime(auxTimeProcessing - timeSlice);
              } else if (randomBinary() == 1){  // Deixar levemente aleatório o momento de troca de segmento
                event->setInstant(prev_termino - timeBeforeChangeSeg);event->setFlag(3);event->setStatus(2);  
                event->setcpuTime(auxTimeProcessing - (timeSlice - timeBeforeChangeSeg));
              } else{
                event->setInstant(prev_termino - (timeBeforeChangeSeg * 2));event->setFlag(3);event->setStatus(3);
                event->setcpuTime(auxTimeProcessing - (timeSlice - (timeBeforeChangeSeg * 2)));
              }
            } else {  // Caso programa tenha menos tempo que o próprio time slice 
              prev_termino = instant + auxTimeProcessing;
              event->setInstant(prev_termino);event->setFlag(4);
              event->setcpuTime(0);
            };

            eventlist.insert(event);
            cout << GREEN << "alocacao de processador do job: " << BOLD << event->getType() << RESET << endl;

          } else if (event->getStatus() == 2 || event->getStatus() == 3) {  // Verificar se vai ocorrer interrupção
            int randomInt = randomBinary();
            auxTimeProcessing = event->getcpuTime();
            // cout << MAGENTA << "numero de aleatorização de interrupção: " << randomInt << RESET << endl;
            if (randomInt == event->getMemoryTree()->current->getNext() || event->getMemoryTree()->current->branch == 1){  
              // Confirma segmento ja carregado

              if (event->getStatus() == 2){
                event->setInstant(instant + timeBeforeChangeSeg);
                event->setcpuTime(auxTimeProcessing - timeBeforeChangeSeg);
              } else {
                event->setInstant(instant + (timeBeforeChangeSeg*2));
                event->setcpuTime(auxTimeProcessing - (timeBeforeChangeSeg*2));
              };
              event->getMemoryTree()->changeNode();
              event->setFlag(4); event->setStatus(1);
              eventlist.insert(event);

            } else {  // Ocorrência de erro de segmento INTERRUPÇÃO DE MEMÓRIA
              waitCPUQ.pop();
              cpuUsage = false;
              eventHandled = event;
              tasksBlocked[event->getType()] = event; // Bloquear a task

              eventFree = new EventNode(0 ,"INTERRUPTION: MEMORY", 3, memoryIMemory, (memoryIMemory->getMaxSegment()*timeSlice - 25));
              eventFree->setStatus(4);  
              eventlist.insert(eventFree);
            }

          } else {  // INTERRUPÇÃO
            eventHandled->setInstant(instant);eventHandled->setFlag(2);eventHandled->setStatus(1);
            eventlist.insert(eventHandled);
            if (!waitCPUQ.empty()){
              eventFree = waitCPUQ.front();
              cout << GREEN << "preparando job " << BOLD << eventFree->getType() << UNBOLD << " para processamento" << RESET << endl;
              eventFree->setInstant(instant);eventFree->setFlag(3);eventFree->setStatus(1);
              eventlist.insert(eventFree);
            }
          }
          break;
        
        // LIBERAÇÃO DE PROCESSADOR
        case 4:
          cout << GREEN << "liberacao de processador do job: " << BOLD << event->getType() << RESET << endl;

          if (event->getcpuTime() == 0){  // Verificar se o processo finalizou
            waitCPUQ.pop();  // Retira da lista
            cpuUsage = false;
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
              cout << GREEN << "preparando job " << BOLD << eventFree->getType() << UNBOLD << " para processamento" << RESET << endl;
              eventFree->setInstant(instant);eventFree->setFlag(3);event->setStatus(1);
              eventlist.insert(eventFree);
            };
          } else {  // Caso o processo ainda não finalizou
            waitCPUQ.pop();
            cpuUsage = false;
            cout << BLUE << "realocando job " << BOLD << event->getType() << UNBOLD << " ao final da fila" << RESET << endl;
            waitCPUQ.push(event);  // Realoca a task no final da fila
            
            eventFree = waitCPUQ.front();  // Retira um item para ser adicionado a lista
            cout << GREEN << "preparando job " << BOLD << eventFree->getType() << UNBOLD << " para processamento" << RESET << endl;
            eventFree->setInstant(instant);eventFree->setFlag(3);
            eventlist.insert(eventFree);
          };
          break;
        
        // LIBERAÇÃO DE MEMÓRIA
        case 5:
          cout << GREEN << "desalocando memoria do job: " << BOLD << event->getType() << RESET << endl;
          event->getMemoryTree()->current = event->getMemoryTree()->head;
          status = memorySistem.Unallocate(event->getType(), event->getMemoryTree(), true);
          if (status == -1){
            if (!waitMemory.empty()){
              eventFree = waitMemory.front();
              cout << GREEN << "preparando job " << BOLD << eventFree->getType() << UNBOLD << " para alocação de memória" << RESET << endl;
              eventFree->setInstant(instant);eventFree->setFlag(2);
              eventlist.insert(eventFree);
            };
            memorySistem.Show();
          } else {
            cout << MAGENTA << "erro ao desalocar o job: " << BOLD << event->getType() << RESET << endl;    
            break;
          };
          event->setInstant(0);event->setFlag(6);
          eventlist.insert(event);
          break;
        
        // SAÍDA DO SISTEMA
        case 6:
          cout << GREEN << "saida do sistema do job: " << BOLD << event->getType() << RESET << endl;
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