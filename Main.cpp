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
#define LIGHT_BLACK   "\033[90m"
#define LIGHT_RED     "\033[91m"
#define LIGHT_GREEN   "\033[92m"
#define LIGHT_YELLOW  "\033[93m"
#define LIGHT_BLUE    "\033[94m"
#define LIGHT_MAGENTA "\033[95m"
#define LIGHT_CYAN    "\033[96m"
#define LIGHT_WHITE   "\033[97m"

using namespace std;
// Debug mode
bool showInstant = false;
bool showInstantOfActions = true;
bool showEventExtract = false;
bool showListOfEvents = false;
bool showMemoryChanges = true;

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
MemoryTree* memoryIMemory = new MemoryTree({1,10,5});

EventList setup() {
  EventList eventlist;
  // memoryTree{code, memory, cpu, disk, device1, device2}
  // code => 1: endline, 2: parent with 1 children, 3: parent with 2 children
  MemoryTree* memorytask1 = new MemoryTree({2,30,20,0,0,0, 1,50,40,0,0,0});
  MemoryTree* memorytask2 = new MemoryTree({2,20,25,0,0,0, 3,30,40,0,2,0, 1,40,100,0,1,2, 2,50,35,0,0,1, 1,35,20,0,2,0});
  MemoryTree* memorytask3 = new MemoryTree({3,40,20,0,1,0, 2,10,10,0,0,2, 1,20,50,0,0,1, 1,15,40,0,2,0});
  MemoryTree* memorytask4 = new MemoryTree({3,40,10,0,0,0, 1,10,30,0,1,0, 1,20,20,0,0,1});

  eventlist.create(20, "Task1", memorytask1, memorytask1->head->getcpuT());
  eventlist.create(20, "Task2", memorytask2, memorytask2->head->getcpuT());
  eventlist.create(220, "Task3", memorytask3, memorytask3->head->getcpuT());
  eventlist.create(240, "Task4", memorytask4, memorytask4->head->getcpuT()); 
  
  return eventlist;
};

void eventEngine(EventList eventlist){
  
  // Variaveis para evento
  EventNode* event = nullptr;
  EventNode* eventFree;
  EventNode* eventHandled;
  // Filas de espera
  queue<EventNode*> waitSpaceMemory;
  queue<EventNode*> waitMemoryQ;
  queue<EventNode*> waitIOPrinterQ;
  queue<EventNode*> waitIOScannerQ;
  queue<EventNode*> waitProcessQ;
  map<string,EventNode*> tasksBlocked;
  queue<EventNode*> waitCPUQ;

  // Componentes do sistema
  Memory memorySystem(memory);
  
  // Variaveis auxiliar do sistema
  bool simulation = true;
  int instant = 0;
  int CPUMARK = 0;
  int prev_termino;
  int status;
  int cputime;
  int timesliceleft;
  bool memoryUsage = false;
  bool cpuUsage = false; 

  while (simulation) {
    instant++;
    CPUMARK--;

    if (showInstant){
      if (cpuUsage){
        cout << GREEN;
      } else {
        cout << RED;
      }
      if (cpuUsage){
        cout << " instante: " << instant;
        cout << ", CPU: " << CPUMARK << RESET << endl;
      } else {
        cout << " instante: " << instant << RESET << endl;
      }
    };

    while (eventlist.head->getInstant() <= instant){
      event = eventlist.takeEvent();

      // Print de retirada da lista
      if (event->getStatus() != 2 && showEventExtract){  // Gambiarra para ajustar algumas interrupções
        cout << WHITE << BOLD << "instante: " << event->getInstant() 
        << ", " << event->getType()
        << ", cpu time: " << event->getcpuTime()
        << ", current memory ID: " << event->getMemoryTree()->current->getID()
        << ", current memory:" << event->getCurrentMemoryNode()->getMemory() << RESET << endl;
      };
      
      //eventlist.display();  // mostrar eventos ainda dentro da lista
  
      int flag = event->getFlag();
      switch(flag){
        // INGRESSO DO JOB AO SISTEMA
        case 1:
          if (showListOfEvents){
            eventlist.display();
          };
          if (showInstantOfActions){cout << GREEN << "instante:" << instant << " ";}
          cout << GREEN << "ingresso ao sistema do job: " << BOLD << event->getType() << RESET << endl;
          event->setInstant(0);event->setFlag(2);
          eventlist.insert(event);
          break;
          
        // ALOCAÇÃO DE MEMÓRIA
        case 2:
          if (event->getStatus() == 0){  // Verifica alocação inicial do programa
            status = memorySystem.Allocate(event->getType() ,event->getMemoryTree(), false);
            if (status == -2){  // Job maior que a memória
              if (showInstantOfActions){cout << MAGENTA << "instante:" << instant << " ";}
              cout << MAGENTA << "nao ha memoria suficiente para alocar o job: " << BOLD << event->getType() << RESET << endl;    
              break;
            } else if (status == -1){  // Não a espaço disponível
              if (showInstantOfActions){cout << YELLOW << "instante:" << instant << " ";}
              cout << YELLOW << "nao ha espaço contiguo disponivel em memoria fisica para o job: " << BOLD << event->getType() << RESET << endl;    
              waitSpaceMemory.push(event);
              break;
            };
            if (waitMemoryQ.empty()){
              if (showInstantOfActions){cout << GREEN << "instante:" << instant << " ";}
              cout << GREEN << "inicio da alocacao de memoria do job: " << BOLD << event->getType() << RESET << endl;
              event->setInstant(instant + timeToMemoryProcess);event->setFlag(2);event->setStatus(1);
              eventlist.insert(event);
            }
            waitMemoryQ.push(event);

          } else if (event->getStatus() == 1){  // Fim do processo de alocação
            status = memorySystem.Allocate(event->getType() ,event->getMemoryTree(), true);
            if (status == -2){  // Job maior que a memória
              if (showInstantOfActions){cout << MAGENTA << "instante:" << instant << " ";}
              cout << MAGENTA << "nao ha memoria suficiente para alocar o job: " << BOLD << event->getType() << RESET << endl;    
              break;
            } else if (status == -1){  // Não a espaço disponível
              if (showInstantOfActions){cout << YELLOW << "instante:" << instant << " ";}
              cout << YELLOW << "nao ha espaço contiguo disponivel em memoria fisica para o job: " << BOLD << event->getType() << RESET << endl;    
              waitSpaceMemory.push(event);
              break;
            } else{  // Alocado 
              if (showInstantOfActions){cout << GREEN << "instante:" << instant << " ";}
              cout << GREEN << BOLD << event->getType() << UNBOLD << ", alocado na memoria em: " << status << RESET << endl; 
            };
            waitMemoryQ.pop();
            event->setInstant(instant);event->setFlag(3);event->setStatus(0);
            eventlist.insert(event);
            
            if (!waitMemoryQ.empty()){
              eventFree = waitMemoryQ.front();
              waitMemoryQ.pop();
              eventFree->setInstant(0);
              eventlist.insert(eventFree);
            }
            if (showMemoryChanges){
              memorySystem.Show();
            }
          }
          break;

        // ALOCAÇÃO DE PROCESSADOR OU FILA DE PROCESSAMENTO
        case 3:
          if (event->getStatus() == 0){  // triagem da task no CPU
            if ((waitCPUQ.size() + tasksBlocked.size()) >= MaxwaitListCPU){  // Número máximo de job processando
              waitProcessQ.push(event);
              break;
            } else {
              event->setStatus(1);
              if ((waitCPUQ.size()) > 0){  // Lista já tem item á frente
                if (showInstantOfActions){cout << GREEN << "instante:" << instant << " ";}
                cout << GREEN << "task : " << BOLD << event->getType() << UNBOLD << " pronta, esperando CPU" << RESET << endl;
                waitCPUQ.push(event);
                break;  
              }
              waitCPUQ.push(event);
            };
          }
          status = event->getStatus();
          if (status == 1){  // Começo das task
            cputime = event->getcpuTime();
            cpuUsage = true;
            CPUMARK = timeSlice;
            if(cputime > timeSlice){  // Se o programa ainda tem tempo maior que o time slice
              prev_termino = timeSlice + instant;
              event->setInstant(prev_termino);event->setFlag(4);
              event->setcpuTime(cputime - timeSlice);

            } else {  // Caso programa tenha menos tempo que o próprio time slice
              prev_termino = instant + cputime;
              event->setInstant(prev_termino);
              if (event->getCurrentMemoryNode()->branch == 0){
                event->setFlag(4);
              } else {
                timesliceleft = timeSlice - cputime;
                event->setFlag(3);event->setStatus(2);
              }
              event->setcpuTime(0);
            };

            eventlist.insert(event);
            if (showInstantOfActions){cout << GREEN << "instante:" << instant << " ";}
            cout << GREEN << "alocacao de processador do job: " << BOLD << event->getType() << RESET << endl;

          } else if (status == 2) {  // Verificar se vai ocorrer interrupção
            int randomInt = 0;
            if (randomInt == event->getCurrentMemoryNode()->getNext() || event->getCurrentMemoryNode()->branch == 1){  
              // Confirma segmento ja carregado
              event->getMemoryTree()->changeNode();
              event->setcpuTime(event->getCurrentMemoryNode()->getcpuT());
              cputime = event->getcpuTime();
              if (cputime >= timesliceleft){  // Caso programa tenha mais tempo que o time slice left
                event->setInstant(instant + timesliceleft);
                event->setcpuTime(cputime - timesliceleft);
                event->setFlag(4); event->setStatus(1);
                eventlist.insert(event);
              } else {  // Caso o programa tenha tempo menor que time slice left
                timesliceleft = timesliceleft - cputime;
                event->setInstant(instant + cputime);
                event->setcpuTime(0);
                if (event->getCurrentMemoryNode()->branch == 0){  // Caso seja o ultimo node
                  event->setFlag(4); event->setStatus(1);
                } else {
                  event->setFlag(3); event->setStatus(2);
                }
                eventlist.insert(event);
              }

            } else {  // Ocorrência de erro de segmento INTERRUPÇÃO DE MEMÓRIA
              waitCPUQ.pop();
              cpuUsage = false;
              eventHandled = event;
              tasksBlocked[event->getType()] = event; // Bloquear a task
              if (showInstantOfActions){cout << RED << "instante:" << instant << " ";}
              cout << RED << "INTERRUPCAO MEMORIA: " << eventHandled->getType() << RESET << endl;
              eventFree = new EventNode(0 ,"INTERRUPTION: MEMORY", memoryIMemory, (memoryIMemory->getMaxSegment()*timeSlice - 25));
              eventFree->setFlag(3);eventFree->setStatus(3);
              eventlist.insert(eventFree);
            }

          } else if (status) {  // INTERRUPÇÃO
            // cout << RED << "INTERRUPCAO DE MEMORIA: " << eventHandled->getType() << RESET << endl;
            if (waitMemoryQ.empty()){
              if (showInstantOfActions){cout << CYAN << "instante:" << instant << " ";}
              cout << CYAN << "iniciando alocacao da " << BOLD << eventHandled->getType() << RESET << endl;
              eventHandled->setInstant(instant + timeToMemoryProcess);eventHandled->setFlag(5);eventHandled->setStatus(1);
              eventlist.insert(eventHandled);
            }
            waitMemoryQ.push(event);
            if (!waitCPUQ.empty()){
              eventFree = waitCPUQ.front();
              if (showInstantOfActions){cout << GREEN << "instante:" << instant << " ";}
              cout << GREEN << "preparando job " << BOLD << eventFree->getType() << UNBOLD << " para processamento" << RESET << endl;
              eventFree->setInstant(instant);eventFree->setFlag(3);eventFree->setStatus(1);
              eventlist.insert(eventFree);
            }
          }
          break;
        
        // LIBERAÇÃO DE PROCESSADOR
        case 4:
          if (showInstantOfActions){cout << BLUE << "instante:" << instant << " ";}
          cout << BLUE << "liberacao de processador do job: " << BOLD << event->getType() << RESET << endl;

          if (event->getcpuTime() == 0){  // Verificar se o processo finalizou
            waitCPUQ.pop();  // Retira da lista
            cpuUsage = false;
            if (!waitProcessQ.empty()){  // Verifica se a lista não está vazia
              eventFree = waitProcessQ.front();
              waitProcessQ.pop();
              eventFree->setInstant(0);eventFree->setFlag(3);
              eventlist.insert(eventFree);
            };
            event->setInstant(0);event->setFlag(6);  // Ajusta as flag para o finalizar a task
            eventlist.insert(event);
            
            if (!waitCPUQ.empty()){  // Verifica se a lista não está vazia
              eventFree = waitCPUQ.front();
              if (showInstantOfActions){cout << GREEN << "instante:" << instant << " ";}
              cout << GREEN << "preparando job " << BOLD << eventFree->getType() << UNBOLD << " para processamento" << RESET << endl;
              eventFree->setInstant(instant);eventFree->setFlag(3);event->setStatus(1);
              eventlist.insert(eventFree);
            };
          } else {  // Caso o processo ainda não finalizou
            waitCPUQ.pop();
            cpuUsage = false;
            if (showInstantOfActions){cout << CYAN << "instante:" << instant << " ";}
            cout << CYAN << "realocando job " << BOLD << event->getType() << UNBOLD << " ao final da fila de CPU" << RESET << endl;
            waitCPUQ.push(event);  // Realoca a task no final da fila
            
            eventFree = waitCPUQ.front();  // Retira um item para ser adicionado a lista
            if (showInstantOfActions){cout << GREEN << "instante:" << instant << " ";}
            cout << GREEN << "preparando job " << BOLD << eventFree->getType() << UNBOLD << " para processamento" << RESET << endl;
            eventFree->setInstant(instant);eventFree->setFlag(3);
            eventlist.insert(eventFree);
          };
          break;
        
        case 5:
          if (event->getStatus() == 1){
            if (showInstantOfActions){cout << YELLOW << "instante:" << instant << " ";}
            cout << YELLOW << "realocacao: " << BOLD << event->getType() << RESET << endl;
            status = memorySystem.Unallocate(event->getType(), event->getMemoryTree(), false);
            memorySystem.Reallocate(event->getType(), event->getMemoryTree(), status);
            if (showMemoryChanges){
              memorySystem.Show();
            }
            event->getMemoryTree()->changeNode();
            event->setcpuTime(event->getCurrentMemoryNode()->getcpuT());
            waitMemoryQ.pop();
            if (!waitMemoryQ.empty()){
              eventFree = waitMemoryQ.front();
              eventFree->setInstant(instant);
              eventlist.insert(eventFree);
            }

            auto it = tasksBlocked.find(event->getType());
            if (it != tasksBlocked.end()){
              tasksBlocked.erase(it);
            };
            
            if (waitCPUQ.empty()){
              event->setInstant(instant);event->setFlag(3);
              eventlist.insert(event);
            }
            waitCPUQ.push(event);
          }
          break;

        // LIBERAÇÃO DE MEMÓRIA
        case 6:
          if (showInstantOfActions){cout << BLUE << "instante:" << instant << " ";}
          cout << BLUE << "desalocando memoria do job: " << BOLD << event->getType() << RESET << endl;
          event->getMemoryTree()->current = event->getMemoryTree()->head;
          status = memorySystem.Unallocate(event->getType(), event->getMemoryTree(), true);
          if (status == -1){
            if (!waitSpaceMemory.empty()){
              eventFree = waitSpaceMemory.front();
              if (showInstantOfActions){cout << GREEN << "instante:" << instant << " ";}
              cout << GREEN << "preparando job " << BOLD << eventFree->getType() << UNBOLD << " para alocação de memória" << RESET << endl;
              eventFree->setInstant(instant);eventFree->setFlag(2);
              eventlist.insert(eventFree);
            };
            if (showMemoryChanges){
              memorySystem.Show();
            }
          } else {
            if (showInstantOfActions){cout << MAGENTA << "instante:" << instant << " ";}
            cout << MAGENTA << "erro ao desalocar o job: " << BOLD << event->getType() << RESET << endl;    
            break;
          };
          event->setInstant(0);event->setFlag(7);
          eventlist.insert(event);
          break;
        
        // SAÍDA DO SISTEMA
        case 7:
          if (showInstantOfActions){cout << BLUE << "instante:" << instant << " ";}
          cout << BLUE << "saida do sistema do job: " << BOLD << event->getType() << RESET << endl;
          if (showListOfEvents){
            eventlist.display();
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
  EventList eventlist = setup();
  eventlist.display();

  // Motor de eventos
  eventEngine(eventlist);
  return 0;
};