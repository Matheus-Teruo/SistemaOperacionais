#include "EventNode.h"
#include "EventList.h"
#include "MemoryTree.h"
#include "Memory.h"
#include "RandomFunction.h"
#include <iostream>
#include <fstream>
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
bool showInstant = true;
bool showInstantOfActions = true;
bool showEventExtract = true;
bool showListOfEvents = true;
bool showMemoryChanges = true;

// prints
struct memoryVec{
  int start;
  int total;
};

struct memoryVecSeg{
  int ID;
  int start;
  int total;
};

struct cpuVec{
  int instant;
  string type;
};

vector<memoryVec> printVecMen1;
vector<memoryVec> printVecMen2;
vector<memoryVec> printVecMen3;
vector<memoryVec> printVecMen4;
vector<vector<memoryVecSeg>> printVecSeg1;
vector<vector<memoryVecSeg>> printVecSeg2;
vector<vector<memoryVecSeg>> printVecSeg3;
vector<vector<memoryVecSeg>> printVecSeg4;

vector<cpuVec> printVecDisp1;
vector<cpuVec> printVecDisp2;
vector<cpuVec> printVecDisp3;
vector<cpuVec> printVecDisp4;

vector<cpuVec> printVecCPU1;
vector<cpuVec> printVecCPU2;
vector<cpuVec> printVecCPU3;
vector<cpuVec> printVecCPU4;
vector<cpuVec> printVecTask1;
vector<cpuVec> printVecTask2;
vector<cpuVec> printVecTask3;
vector<cpuVec> printVecTask4;

// PC resources
int memory = 300; //Mb
int timeToMemoryProcess = 5;
int MaxDevice1 = 2;
int MaxDevice2 = 2;
int timeWaitDevice1 = 5;
int timeWaitDevice2 = 8;
int MaxmultiProg = 5;  // ??? NÃO SEI SE USO AINDA
int MaxwaitListCPU = 3;
int timeSlice = 30;
// Uma fila única para o disco físico
// Arquivos diversas filas, cada uma para controlar o acesso a um arquivo específico em uso

// Interrupções
MemoryTree* memoryIMemory = new MemoryTree({1,10,5,0,0,0});
MemoryTree* memoryIDevice = new MemoryTree({1,10,5,0,0,0});

EventList setup() {
  EventList eventlist;
  // memoryTree{code, memory, cpu, disk, device1, device2}
  // code => 1: endline, 2: parent with 1 children, 3: parent with 2 children
  MemoryTree* memorytask1 = new MemoryTree({2,30,20,0,0,0, 1,50,40,0,0,0});
  MemoryTree* memorytask2 = new MemoryTree({2,20,25,0,0,0, 3,30,40,0,2,0, 1,40,100,0,1,2, 2,50,35,0,0,1, 1,35,20,0,2,0});
  MemoryTree* memorytask3 = new MemoryTree({3,40,20,0,1,0, 2,10,80,0,0,0, 1,20,50,0,0,1, 1,15,40,0,2,0});
  MemoryTree* memorytask4 = new MemoryTree({3,40,10,0,0,0, 1,10,30,0,1,0, 1,20,20,0,0,1});

  eventlist.create(20, "Task1", memorytask1, memorytask1->head->getcpuT());
  eventlist.create(20, "Task2", memorytask2, memorytask2->head->getcpuT());
  eventlist.create(30, "Task3", memorytask3, memorytask3->head->getcpuT());
  eventlist.create(40, "Task4", memorytask4, memorytask4->head->getcpuT()); 
  
  return eventlist;
};

void saveMenToCSV(const string& filename, const vector<memoryVec> data) {
    ofstream file(filename);

    if (file.is_open()) {
        for (const auto& element : data) {
            file << element.start << "," << element.total << "\n";
        }
        file.close();
        cout << "Data saved to " << filename << endl;
    }
    else {
        cerr << "Unable to open file: " << filename << endl;
    }
}

void saveSegmentToCSV(const string& filename, const vector<vector<memoryVecSeg>> data) {
    ofstream file(filename);

    if (file.is_open()) {
        for (const auto row : data) {
            for (size_t i = 0; i < row.size(); ++i) {
                file << row[i].ID << "," << row[i].start << "," << row[i].total;
                if (i != row.size() - 1) {
                    file << ",";
                }
            }
            file << "\n";
        }
        file.close();
        cout << "Data saved to " << filename << endl;
    }
    else {
        cerr << "Unable to open file: " << filename << endl;
    }
}

void saveCPUToCSV(const string& filename, const vector<cpuVec> data) {
    ofstream file(filename);

    if (file.is_open()) {
        for (const auto& element : data) {
            file << element.type << "," << element.instant << "\n";
        }
        file.close();
        cout << "Data saved to " << filename << endl;
    }
    else {
        cerr << "Unable to open file: " << filename << endl;
    }
}

void eventEngine(EventList eventlist){
  
  // Variaveis para evento
  EventNode* event = nullptr;
  EventNode* eventFree;
  EventNode* eventHandled;
  // Filas de espera
  queue<EventNode*> waitSpaceMemory;
  queue<EventNode*> waitMemoryQ;

  queue<EventNode*> waitIOPrinterQ;
  map<string,EventNode*> useIOPrinterQ;
  queue<EventNode*> waitPrinterBusyQ;

  queue<EventNode*> waitIOScannerQ;
  map<string,EventNode*> useIOScannerQ;
  queue<EventNode*> waitScannerBusyQ;

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
  bool device1Used = false;
  bool device2Used = false;
  int device1;
  int device2;
  int totaldevice;
  int timesliceleft;
  bool memoryUsage = false;
  bool cpuUsage = false; 
  int printtask1 = 0;
  int printtask2 = 0;
  int printtask3 = 0;
  int printtask4 = 0;

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
        cout << " instante:" << instant;
        cout << ", CPU:" << CPUMARK << RESET << endl;
      } else {
        cout << " instante:" << instant << RESET << endl;
      }
    };

    while (simulation && eventlist.head->getInstant() <= instant){
      event = eventlist.takeEvent();

      // Print de retirada da lista
      if (event->getStatus() != 2 && event->getStatus() != 4 && showEventExtract){  // Gambiarra para ajustar algumas interrupções
        cout << WHITE << BOLD <<
        "instante: " << instant
        << ", chegada: " << event->getInstant() 
        << ", " << event->getType()
        << ", cpu time: " << event->getcpuTime()
        << ", current memory ID: " << event->getMemoryTree()->current->getID()
        << ", flag:" << event->getFlag()
        << ", status:" << event->getStatus()
        << ", printer:" << event->getCurrentMemoryNode()->getdevice1()
        << ", scanner:" << event->getCurrentMemoryNode()->getdevice2() << RESET << endl;
      };
      
      eventlist.display();  // mostrar eventos ainda dentro da lista
  
      int flag = event->getFlag();
      switch(flag){
        // INGRESSO DO JOB AO SISTEMA
        case 1:  // ==============================================================================================
          if (event->getType() == "Task1"){  // PRINT ============================
            printVecTask1.push_back(cpuVec{instant,"arrival"});
            printVecCPU1.push_back(cpuVec{instant,"arrival"});
          }else if (event->getType() == "Task2"){
            printVecTask2.push_back(cpuVec{instant,"arrival"});
            printVecCPU2.push_back(cpuVec{instant,"arrival"});
          }else if (event->getType() == "Task3"){
            printVecTask3.push_back(cpuVec{instant,"arrival"});
            printVecCPU3.push_back(cpuVec{instant,"arrival"});
          }else if (event->getType() == "Task4"){
            printVecTask4.push_back(cpuVec{instant,"arrival"});
            printVecCPU4.push_back(cpuVec{instant,"arrival"});
          };
          
          if (showListOfEvents){
            eventlist.display();
          };
          if (showInstantOfActions){cout << LIGHT_GREEN << "instante:" << instant << " ";}
          cout << LIGHT_GREEN << "ingresso ao sistema do job: " << BOLD << event->getType() << RESET << endl;
          event->setInstant(0);event->setFlag(2);
          eventlist.insert(event);
          break;
          
        // ALOCAÇÃO DE MEMÓRIA
        case 2:  // ==============================================================================================
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
            if (waitMemoryQ.empty()){  // Execução imediata caso esteja vazio
              if (showInstantOfActions){cout << LIGHT_GREEN << "instante:" << instant << " ";}
              cout << LIGHT_GREEN << "inicio da alocacao de memoria do job: " << BOLD << event->getType() << RESET << endl;
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
              if (showInstantOfActions){cout << LIGHT_GREEN << "instante:" << instant << " ";}
              cout << LIGHT_GREEN << BOLD << event->getType() << UNBOLD << ", alocado na memoria em: " << status << RESET << endl; 
            };
            waitMemoryQ.pop();
            event->setInstant(0);event->setFlag(3);event->setStatus(0);
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

        // ALLOCAÇÃO DE DISPOSITIVOS
        case 3:  // ==============================================================================================
          if (event->getAllocation().device1 == 0){  // Device 1 Printer
            if (useIOPrinterQ.size() >= MaxDevice1){  // Verifica se existe mais dispositivos alocados do que o máximo
              cout << LIGHT_GREEN << "maxima alocacao do dispositivo 1, " << BOLD << event->getType() << UNBOLD << " na fila de espera" << RESET << endl;
              waitIOPrinterQ.push(event);
            } else {  // Alocação do dispositivo 1
              cout << LIGHT_GREEN << "impressora reservada para " << BOLD << event->getType() << RESET << endl;
              event->setAlDevice1(1);
              useIOPrinterQ[event->getType()] = event;
            }
          }
          if (event->getAllocation().device2 == 0){  // Device 2 Scanner
            if (useIOScannerQ.size() >= MaxDevice2){  // Verifica se existe mais dispositivos alocados do que o máximo
              cout << LIGHT_GREEN << "maxima alocacao do dispositivo 2, " << BOLD << event->getType() << UNBOLD << " na fila de espera" << RESET << endl;
              waitIOScannerQ.push(event);
            } else {  // Alocação do dispositivo 2
              cout << LIGHT_GREEN << "scanner reservado para " << BOLD << event->getType() << RESET << endl;
              event->setAlDevice2(1);
              useIOScannerQ[event->getType()] = event;
            }
          }
          if (event->getAllocation().device1 != 0 && event->getAllocation().device2 != 0){  // verifica se os dispositivos foram alocados
            event->setInstant(0);
            event->setFlag(4);event->setStatus(0);
            eventlist.insert(event);
          }
          break;

        // ALOCAÇÃO DE PROCESSADOR OU FILA DE PROCESSAMENTO
        case 4:  // ==============================================================================================
          if (event->getStatus() == 0){  // triagem da task no CPU
            if ((waitCPUQ.size() + tasksBlocked.size()) >= MaxwaitListCPU){  // Número máximo de job processando
              waitProcessQ.push(event);
              break;
            } else {
              event->setStatus(1);
              if ((waitCPUQ.size()) > 0){  // Já existe task ocorrendo, entrand na fila de espera
                if (showInstantOfActions){cout << LIGHT_GREEN << "instante:" << instant << " ";}
                cout << LIGHT_GREEN << BOLD << event->getType() << UNBOLD << " pronta, esperando CPU" << RESET << endl;
                waitCPUQ.push(event);
                break;  
              }
              waitCPUQ.push(event);
            };
          }
          status = event->getStatus();
          if (status == 1){  // Começo das task

            if (event->getType() == "Task1"){  // PRINT ====================
              if (printtask1 != 1) {
                printtask1 = 1;
                printVecTask1.push_back(cpuVec{instant,"cpuStart"});
              }
              printVecCPU1.push_back(cpuVec{instant,"cpuStart"});
            }else if (event->getType() == "Task2"){
              if (printtask2 != 1) {
                printtask2 = 1;
                printVecTask2.push_back(cpuVec{instant,"cpuStart"});
              }
              printVecCPU2.push_back(cpuVec{instant,"cpuStart"});
            }else if (event->getType() == "Task3"){
              if (printtask3 != 1) {
                printtask3 = 1;
                printVecTask3.push_back(cpuVec{instant,"cpuStart"});
              }
              printVecCPU3.push_back(cpuVec{instant,"cpuStart"});
            }else if (event->getType() == "Task4"){
              if (printtask4 != 1) {
                printtask4 = 1;
                printVecTask4.push_back(cpuVec{instant,"cpuStart"});
              }
              printVecCPU4.push_back(cpuVec{instant,"cpuStart"});
            };

            cputime = event->getcpuTime();
            totaldevice = event->getCurrentMemoryNode()->getdevice1() + event->getCurrentMemoryNode()->getdevice2();
            cputime = cputime / (totaldevice + 1);  // Dividir o tempo em partes iguais para entrada e saída
            cpuUsage = true;
            CPUMARK = timeSlice;
            if(cputime > timeSlice){  // Se o programa tem tempo maior que o time slice
              prev_termino = timeSlice + instant;
              event->setInstant(prev_termino);
              event->setFlag(5);
              event->setcpuTime(event->getcpuTime() - timeSlice);
              if (totaldevice > 0){  // caso tenha interrupção de dispositivo
                event->setFlag(4);event->setStatus(4);
              }

            } else {  // Caso programa tenha tempo menor que o time slice
              prev_termino = instant + cputime;
              event->setInstant(prev_termino);
              if (event->getCurrentMemoryNode()->branch == 0){  // Caso seja o ultimo node
                event->setFlag(5);
              } else {
                timesliceleft = timeSlice - cputime;
                event->setFlag(4);event->setStatus(2);
              }
              if (totaldevice > 0){  // caso tenha interrupção de dispositivo
                event->setFlag(4);event->setStatus(4);
                event->setcpuTime(event->getcpuTime() - cputime);
              } else {
                event->setcpuTime(0);
              }
            };

            eventlist.insert(event);
            if (showInstantOfActions){cout << LIGHT_GREEN << "instante:" << instant << " ";}
            cout << LIGHT_GREEN << "alocacao de processador do job: " << BOLD << event->getType() << RESET << endl;

          } else if (status == 2) {  // Verificar se irá ocorrerá interrupção de memória
            int randomInt = 0;
            if (randomInt == event->getCurrentMemoryNode()->getNext() || event->getCurrentMemoryNode()->branch == 1){  
              // Segmnento requerido já carregado na memória
              cout << CYAN << "Troca de segmento em " << BOLD << event->getType() << RESET << endl;
              event->getMemoryTree()->changeNode();
              event->setcpuTime(event->getCurrentMemoryNode()->getcpuT());
              cputime = event->getcpuTime();
              totaldevice = event->getCurrentMemoryNode()->getdevice1() + event->getCurrentMemoryNode()->getdevice2();
              cputime = cputime / (totaldevice + 1);  // Dividir o tempo em partes iguais para entrada e saída
              if (cputime >= timesliceleft){  // Caso programa tenha mais tempo que o time slice left
                event->setInstant(instant + timesliceleft);
                event->setcpuTime(event->getcpuTime() - timesliceleft);
                event->setFlag(5); event->setStatus(1);
                eventlist.insert(event);
                
                if (totaldevice > 0){  // caso tenha interrupção de dispositivo
                  event->setFlag(4);event->setStatus(4);
                }
              } else {  // Caso o programa tenha tempo menor que time slice left
                timesliceleft = timesliceleft - cputime;
                event->setInstant(instant + cputime);
                if (event->getCurrentMemoryNode()->branch == 0){  // Caso seja o ultimo node
                  event->setFlag(5); event->setStatus(1);
                } else {
                  event->setFlag(4); event->setStatus(2);
                }
                if (totaldevice > 0){  // caso tenha interrupção de dispositivo
                  event->setFlag(4);event->setStatus(4);
                  event->setcpuTime(event->getcpuTime() - cputime);
                } else {
                  event->setcpuTime(0);
                }

                eventlist.insert(event);
              }

            } else {  // Ocorrência de erro de segmento INTERRUPÇÃO DE MEMÓRIA

              if (event->getType() == "Task1"){  // PRINT ====================
                printVecCPU1.push_back(cpuVec{instant,"cpuEnd"});
              }else if (event->getType() == "Task2"){
                printVecCPU2.push_back(cpuVec{instant,"cpuEnd"});
              }else if (event->getType() == "Task3"){
                printVecCPU3.push_back(cpuVec{instant,"cpuEnd"});
              }else if (event->getType() == "Task4"){
                printVecCPU4.push_back(cpuVec{instant,"cpuEnd"});
              };

              waitCPUQ.pop();
              cpuUsage = false;
              eventHandled = event;
              event->setStatus(3);
              tasksBlocked[event->getType()] = event; // Bloquear a task
              eventFree = new EventNode(0 ,"INTERRUPTION: MEMORY", memoryIMemory, 0);
              eventFree->setFlag(4);eventFree->setStatus(3);
              eventlist.insert(eventFree);
            }

          } else if (status == 3) {  // INTERRUPÇÃO DE MEMÓRIA
            if (event->getType() == "INTERRUPTION: MEMORY"){
              if (showInstantOfActions){cout << RED << "instante:" << instant << " ";}
              cout << RED << "INTERRUPCAO MEMORIA: " << BOLD << eventHandled->getType() << RESET << endl;
            } else {
              eventHandled = event;
            }
            if (waitMemoryQ.empty()){  // Caso não tenha nenhum programa usando memória, processa direto
              if (showInstantOfActions){cout << CYAN << "instante:" << instant << " ";}
              cout << CYAN << "iniciando alocacao da " << BOLD << eventHandled->getType() << RESET << endl;
              eventHandled->setInstant(instant + timeToMemoryProcess);eventHandled->setFlag(6);eventHandled->setStatus(1);
              eventlist.insert(eventHandled);
            }
            waitMemoryQ.push(eventHandled);
            if (!waitCPUQ.empty()){  // Caso a CPU ja esteja utilizada
              eventFree = waitCPUQ.front();
              if (showInstantOfActions){cout << LIGHT_GREEN << "instante:" << instant << " ";}
              cout << LIGHT_GREEN << "preparando job " << BOLD << eventFree->getType() << UNBOLD << " para processamento" << RESET << endl;
              eventFree->setInstant(instant);eventFree->setFlag(4);eventFree->setStatus(1);
              eventlist.insert(eventFree);
            }

          } else  if (status == 4){  // Preparar interrupção de dispositivo
            waitCPUQ.pop();
            cpuUsage = false;

            if (event->getType() == "Task1"){  // PRINT ====================
              printVecCPU1.push_back(cpuVec{instant,"cpuEnd"});
            }else if (event->getType() == "Task2"){
              printVecCPU2.push_back(cpuVec{instant,"cpuEnd"});
            }else if (event->getType() == "Task3"){
              printVecCPU3.push_back(cpuVec{instant,"cpuEnd"});
            }else if (event->getType() == "Task4"){
              printVecCPU4.push_back(cpuVec{instant,"cpuEnd"});
            };

            eventHandled = event;
            event->setStatus(5);
            tasksBlocked[event->getType()] = event; // Bloquear a task
            eventFree = new EventNode(0 ,"INTERRUPTION: DEVICE", memoryIMemory, 0);
            eventFree->setFlag(4);eventFree->setStatus(5);
            eventlist.insert(eventFree);

          } else if (status == 5){  // INTERRUPÇÃO DE DEVICE
            if (event->getType() == "INTERRUPTION: DEVICE"){
              if (showInstantOfActions){cout << RED << "instante:" << instant << " ";}
              cout << RED << "INTERRUPCAO DISPOSITIVO: " << BOLD << eventHandled->getType() << RESET << endl;
              
              if (!waitCPUQ.empty()){  // Caso a CPU ja esteja utilizada
                eventFree = waitCPUQ.front();
                if (showInstantOfActions){cout << LIGHT_GREEN << "instante:" << instant << " ";}
                cout << LIGHT_GREEN << "preparando job " << BOLD << eventFree->getType() << UNBOLD << " para processamento" << RESET << endl;
                eventFree->setInstant(instant);eventFree->setFlag(4);eventFree->setStatus(1);
                eventlist.insert(eventFree);
              }
            } else {
              eventHandled = event;
            }
            device1 = eventHandled->getCurrentMemoryNode()->getdevice1();
            device2 = eventHandled->getCurrentMemoryNode()->getdevice2();
            //cout << "device 1: " << device1 << ", 2: " << device2 << endl;
            if (device1 > 0 && (randomBinary() == 0 || device2 == 0)){  // Ajusta para uso do dispositivo 1 PRINTER
              if (!device1Used){
                eventHandled->getCurrentMemoryNode()->setdevice1(device1 - 1);
                if (showInstantOfActions){cout << CYAN << "instante:" << instant << " ";}
                cout << CYAN << "Tratando entrada e saida dispositivo 1, Impressora, para " << BOLD << eventHandled->getType() << RESET << endl;
                eventHandled->setStatus(0);
                eventHandled->setInstant(instant + timeWaitDevice1);
                device1Used = true;

                if (eventHandled->getType() == "Task1"){  // PRINT ====================
                  printVecDisp1.push_back(cpuVec{instant,"device1Start"});
                }else if (eventHandled->getType() == "Task2"){
                  printVecDisp2.push_back(cpuVec{instant,"device1Start"});
                }else if (eventHandled->getType() == "Task3"){
                  printVecDisp3.push_back(cpuVec{instant,"device1Start"});
                }else if (eventHandled->getType() == "Task4"){
                  printVecDisp4.push_back(cpuVec{instant,"device1Start"});
                };
                
              } else {
                //cout << "device 1: " << device1 << endl;
                eventHandled->setStatus(2);
                eventHandled->setInstant(0);
              }
            } else if (device2 > 0){  // Ajusta para uso do dispositivo 2 SCANNER
              if (!device2Used){
                eventHandled->getCurrentMemoryNode()->setdevice2(device2 - 1);
                if (showInstantOfActions){cout << LIGHT_GREEN << "instante:" << instant << " ";}
                cout << CYAN << "Tratando entrada e saida dispositivo 2, Scanner, para " << BOLD << eventHandled->getType() << RESET << endl;
                eventHandled->setStatus(3);
                eventHandled->setInstant(instant + timeWaitDevice2);
                device2Used = true;

                if (eventHandled->getType() == "Task1"){  // PRINT ====================
                  printVecDisp1.push_back(cpuVec{instant,"device2Start"});
                }else if (eventHandled->getType() == "Task2"){
                  printVecDisp2.push_back(cpuVec{instant,"device2Start"});
                }else if (eventHandled->getType() == "Task3"){
                  printVecDisp3.push_back(cpuVec{instant,"device2Start"});
                }else if (eventHandled->getType() == "Task4"){
                  printVecDisp4.push_back(cpuVec{instant,"device2Start"});
                };

              } else {
                //cout << "device 2: " << device2 << endl;
                eventHandled->setStatus(4);
                eventHandled->setInstant(0);
              }
            }
            eventHandled->setFlag(6);
            eventlist.insert(eventHandled);
          }
          break;
        
        // LIBERAÇÃO DE PROCESSADOR
        case 5:  // ==============================================================================================
          if (showInstantOfActions){cout << BLUE << "instante:" << instant << " ";}
          cout << BLUE << "liberacao de processador do job: " << BOLD << event->getType() << RESET << endl;

          if (event->getcpuTime() == 0){  // Verificar se o processo finalizou
            waitCPUQ.pop();  // Retira da lista
            cpuUsage = false;

            if (event->getType() == "Task1"){  // PRINT ====================
              printVecTask1.push_back(cpuVec{instant,"cpuEnd"});
              printVecCPU1.push_back(cpuVec{instant,"cpuEnd"});
            }else if (event->getType() == "Task2"){
              printVecTask2.push_back(cpuVec{instant,"cpuEnd"});
              printVecCPU2.push_back(cpuVec{instant,"cpuEnd"});
            }else if (event->getType() == "Task3"){
              printVecTask3.push_back(cpuVec{instant,"cpuEnd"});
              printVecCPU3.push_back(cpuVec{instant,"cpuEnd"});
            }else if (event->getType() == "Task4"){
              printVecTask4.push_back(cpuVec{instant,"cpuEnd"});
              printVecCPU4.push_back(cpuVec{instant,"cpuEnd"});
            };

            if (!waitProcessQ.empty()){  // Verifica se a lista não está vazia
              eventFree = waitProcessQ.front();
              waitProcessQ.pop();
              eventFree->setInstant(0);eventFree->setFlag(4);
              eventlist.insert(eventFree);
            };
            event->setInstant(0);event->setFlag(7);  // Ajusta as flag para o finalizar a task
            eventlist.insert(event);
            
            if (!waitCPUQ.empty()){  // Verifica se a lista não está vazia
              eventFree = waitCPUQ.front();
              if (showInstantOfActions){cout << LIGHT_GREEN << "instante:" << instant << " ";}
              cout << LIGHT_GREEN << "preparando job " << BOLD << eventFree->getType() << UNBOLD << " para processamento" << RESET << endl;
              eventFree->setInstant(instant);eventFree->setFlag(4);event->setStatus(1);
              eventlist.insert(eventFree);
            };
          } else {  // Caso o processo ainda não finalizou
            waitCPUQ.pop();
            cpuUsage = false;

            if (event->getType() == "Task1"){  // PRINT ====================
              printVecCPU1.push_back(cpuVec{instant,"cpuEnd"});
            }else if (event->getType() == "Task2"){
              printVecCPU2.push_back(cpuVec{instant,"cpuEnd"});
            }else if (event->getType() == "Task3"){
              printVecCPU3.push_back(cpuVec{instant,"cpuEnd"});
            }else if (event->getType() == "Task4"){
              printVecCPU4.push_back(cpuVec{instant,"cpuEnd"});
            };

            if (showInstantOfActions){cout << CYAN << "instante:" << instant << " ";}
            cout << CYAN << "realocando job " << BOLD << event->getType() << UNBOLD << " ao final da fila de CPU" << RESET << endl;
            waitCPUQ.push(event);  // Realoca a task no final da fila
            
            eventFree = waitCPUQ.front();  // Retira um item para ser adicionado a lista
            if (showInstantOfActions){cout << LIGHT_GREEN << "instante:" << instant << " ";}
            cout << LIGHT_GREEN << "preparando job " << BOLD << eventFree->getType() << UNBOLD << " para processamento" << RESET << endl;
            eventFree->setInstant(instant);eventFree->setFlag(4);
            eventlist.insert(eventFree);
          };
          break;
        
        // TRATAMENTO DE TASK BLOQUEADA
        case 6:  // ==============================================================================================
          if (event->getStatus() == 1){  // Tratamento de memória
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
              event->setInstant(instant);event->setFlag(4);
              eventlist.insert(event);
            }
            waitCPUQ.push(event);


          } else if (event->getStatus() == 0){  // Tratamento do Device 1
    	      if (event->getType() == "Task1"){  // PRINT ====================
              printVecDisp1.push_back(cpuVec{instant,"device1End"});
            }else if (event->getType() == "Task2"){
              printVecDisp2.push_back(cpuVec{instant,"device1End"});
            }else if (event->getType() == "Task3"){
              printVecDisp3.push_back(cpuVec{instant,"device1End"});
            }else if (event->getType() == "Task4"){
              printVecDisp4.push_back(cpuVec{instant,"device1End"});
            };

            device1Used = false;
            if (showInstantOfActions){cout << LIGHT_GREEN << "instante:" << instant << " ";}
            cout << LIGHT_GREEN << "finalizacao de entrada e saida de impressora, dispositivo 1, resolvido para " << event->getType() << endl;
            event->setInstant(0);
            event->setFlag(4); event->setStatus(1);

            auto it = tasksBlocked.find(event->getType());  // Retira de task bloqueada
            if (it != tasksBlocked.end()){
              tasksBlocked.erase(it);
            };

            if (!waitPrinterBusyQ.empty()){  // Ajusta task esperando por dispositivo 1 impressora
              eventFree = waitPrinterBusyQ.front();
              waitPrinterBusyQ.pop();
              eventFree->setInstant(instant);
              eventFree->setFlag(4);eventFree->setStatus(5);
              eventlist.insert(eventFree);
            }

            if (waitCPUQ.empty()){  // Já existe task ocorrendo, entra na fila de espera
              event->setInstant(instant);event->setFlag(4);
              eventlist.insert(event);
            }
            waitCPUQ.push(event);

          } else if (event->getStatus() == 3){  // Tratamento do device 2
            if (event->getType() == "Task1"){  // PRINT ====================
              printVecDisp1.push_back(cpuVec{instant,"device2End"});
            }else if (event->getType() == "Task2"){
              printVecDisp2.push_back(cpuVec{instant,"device2End"});
            }else if (event->getType() == "Task3"){
              printVecDisp3.push_back(cpuVec{instant,"device2End"});
            }else if (event->getType() == "Task4"){
              printVecDisp4.push_back(cpuVec{instant,"device2End"});
            };

            device2Used = false;
            if (showInstantOfActions){cout << LIGHT_GREEN << "instante:" << instant << " ";}
            cout << LIGHT_GREEN << "finalizacao de entrada e saida de scanner, dispositivo 2, resolvido para " << event->getType() << endl;
            event->setInstant(0);
            event->setFlag(4); event->setStatus(1);

            auto it = tasksBlocked.find(event->getType());  // Retira de task bloqueada
            if (it != tasksBlocked.end()){
              tasksBlocked.erase(it);
            };

            if (!waitScannerBusyQ.empty()){  // Ajusta task esperando por dispositivo 2 scanner
              eventFree = waitScannerBusyQ.front();
              waitScannerBusyQ.pop();
              eventFree->setInstant(instant);
              eventFree->setFlag(4);eventFree->setStatus(5);
              eventlist.insert(eventFree);
            }

            if (waitCPUQ.empty()){  // Já existe task ocorrendo, entrand na fila de espera
              event->setInstant(instant);event->setFlag(4);
              eventlist.insert(event);
            }
            waitCPUQ.push(event);

          } else if (event->getStatus() == 2){  // Lista de espera
            if (showInstantOfActions){cout << BLUE << "instante:" << instant << " ";}
            cout << BLUE << BOLD << event->getType() << UNBOLD << " esperando dispositivo 1, impressora" << RESET << endl;
            waitPrinterBusyQ.push(event);
          } else if (event->getStatus() == 4){  // Lista de espera
            if (showInstantOfActions){cout << BLUE << "instante:" << instant << " ";}
            cout << BLUE << BOLD << event->getType() << UNBOLD << " esperando dispositivo 2, scanner" << RESET << endl;
            waitScannerBusyQ.push(event);
          }
          break;

        // DESALOCAR DISPOSITIVOS
        case 7:  // ==============================================================================================
          status = 0; 
          if (event->getAllocation().device1 == 1){ // Desalocando dispositivo 1 Printer
            auto it = useIOPrinterQ.find(event->getType());
            if (it != useIOPrinterQ.end()){
              if (showInstantOfActions){cout << BLUE << "instante:" << instant << " ";}
              cout << BLUE << "desalocando dispositivo 1, Scanner, para" << BOLD << event->getType() << RESET << endl;
              useIOPrinterQ.erase(it);
            } else {
              if (showInstantOfActions){cout << MAGENTA << "instante:" << instant << " ";}
              cout << MAGENTA << "erro ao desalocar dispositivo 1, Scanner, para" << BOLD << event->getType() << RESET << endl; 
              break;
            };
            if (!waitIOPrinterQ.empty()){
              eventFree = waitIOPrinterQ.front();
              if (showInstantOfActions){cout << CYAN << "instante:" << instant << " ";}
              cout << CYAN << "preparando dispositivo 1, Scanner, para alocar a " << BOLD << eventFree->getType() << RESET << endl;
              waitIOPrinterQ.pop();
              status = 1;
            }
          }
          if (event->getAllocation().device2 == 1){ // Desalocando dispositivo 2 Scanner
            auto it = useIOScannerQ.find(event->getType());
            if (it != useIOScannerQ.end()){
              if (showInstantOfActions){cout << BLUE << "instante:" << instant << " ";}
              cout << BLUE << "desalocando dispositivo 2, Scanner, para" << BOLD << event->getType() << RESET << endl;
              useIOScannerQ.erase(it);
            } else{
              if (showInstantOfActions){cout << MAGENTA << "instante:" << instant << " ";}
              cout << MAGENTA << "erro ao desalocar dispositivo 2, Scanner, para" << BOLD << event->getType() << RESET << endl;
              break;
            };
            if (!waitIOScannerQ.empty()){
              eventFree = waitIOScannerQ.front();
              if (showInstantOfActions){cout << CYAN << "instante:" << instant << " ";}
              cout << CYAN << "preparando dispositivo 2, Scanner, para alocar a " << BOLD << eventFree->getType() << RESET << endl;
              waitIOScannerQ.pop();
              status = 1;
            }
          }
          if (status == 1){
            eventFree->setInstant(instant);
            eventFree->setFlag(3);
            eventlist.insert(eventFree);
          }
          event->setInstant(0);
          event->setFlag(8);
          eventlist.insert(event);
          break;

        // LIBERAÇÃO DE MEMÓRIA
        case 8:  // ==============================================================================================
          if (showInstantOfActions){cout << BLUE << "instante:" << instant << " ";}
          cout << BLUE << "desalocando memoria do job: " << BOLD << event->getType() << RESET << endl;
          event->getMemoryTree()->current = event->getMemoryTree()->head;
          status = memorySystem.Unallocate(event->getType(), event->getMemoryTree(), true);
          if (status == -1){  // Desalocamento de memória corretamente
            if (!waitSpaceMemory.empty()){
              eventFree = waitSpaceMemory.front();
              waitSpaceMemory.pop();
              if (showInstantOfActions){cout << LIGHT_GREEN << "instante:" << instant << " ";}
              cout << LIGHT_GREEN << "preparando job " << BOLD << eventFree->getType() << UNBOLD << " para alocacao de memoria" << RESET << endl;
              eventFree->setInstant(instant);eventFree->setFlag(2);
              eventlist.insert(eventFree);
            };
            if (showMemoryChanges){
              memorySystem.Show();
            }
          } else {  // Caso de erro
            if (showInstantOfActions){cout << MAGENTA << "instante:" << instant << " ";}
            cout << MAGENTA << "erro ao desalocar o job: " << BOLD << event->getType() << RESET << endl;    
            break;
          };
          event->setInstant(0);event->setFlag(9);
          eventlist.insert(event);
          break;
        
        // SAÍDA DO SISTEMA
        case 9:  // ==============================================================================================
          if (showInstantOfActions){cout << BLUE << "instante:" << instant << " ";}
          cout << BLUE << "saida do sistema do job: " << BOLD << event->getType() << RESET << endl;
          if (showListOfEvents){
            eventlist.display();
          }
          if (eventlist.head == nullptr){
            simulation = false;
          }
            break;
      }
    }

    // Print ==========================================================
    memoryVec pMenAT1 = memoryVec{0, 0};
    memoryVec pMenAT2 = memoryVec{0, 0};
    memoryVec pMenAT3 = memoryVec{0, 0};
    memoryVec pMenAT4 = memoryVec{0, 0};

    for (const logicalMemory lm : memorySystem.getAllocated()) {
      if (lm.type == "Task1"){
        pMenAT1 = memoryVec{lm.start,lm.total};
      } else if (lm.type == "Task2"){
        pMenAT2 = memoryVec{lm.start,lm.total};
      } else if (lm.type == "Task3"){
        pMenAT3 = memoryVec{lm.start,lm.total};
      } else if (lm.type == "Task4"){
        pMenAT4 = memoryVec{lm.start,lm.total};
      }
    }

    vector<memoryVecSeg> rowMen1;
    vector<memoryVecSeg> rowMen2;
    vector<memoryVecSeg> rowMen3;
    vector<memoryVecSeg> rowMen4;

    memoryVecSeg pMenST1OL = memoryVecSeg{0, 0, 0};
    memoryVecSeg pMenST1ID0 = memoryVecSeg{0, 0, 0};
    memoryVecSeg pMenST1ID1 = memoryVecSeg{0, 0, 0};
    
    memoryVecSeg pMenST2OL = memoryVecSeg{0, 0, 0};
    memoryVecSeg pMenST2ID0 = memoryVecSeg{0, 0, 0};
    memoryVecSeg pMenST2ID1 = memoryVecSeg{0, 0, 0};
    memoryVecSeg pMenST2ID2 = memoryVecSeg{0, 0, 0};
    memoryVecSeg pMenST2ID3 = memoryVecSeg{0, 0, 0};
    memoryVecSeg pMenST2ID4 = memoryVecSeg{0, 0, 0};
    
    memoryVecSeg pMenST3OL = memoryVecSeg{0, 0, 0};
    memoryVecSeg pMenST3ID0 = memoryVecSeg{0, 0, 0};
    memoryVecSeg pMenST3ID1 = memoryVecSeg{0, 0, 0};
    memoryVecSeg pMenST3ID2 = memoryVecSeg{0, 0, 0};
    memoryVecSeg pMenST3ID3 = memoryVecSeg{0, 0, 0};
    
    memoryVecSeg pMenST4OL = memoryVecSeg{0, 0, 0};
    memoryVecSeg pMenST4ID0 = memoryVecSeg{0, 0, 0};
    memoryVecSeg pMenST4ID1 = memoryVecSeg{0, 0, 0};
    memoryVecSeg pMenST4ID2 = memoryVecSeg{0, 0, 0};

    for (const auto lm : memorySystem.getPointers()){
      if (lm.second.type == "Task1"){
        if (lm.second.cputimeseg == 0){
          pMenST1OL = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        }else if (lm.second.ID == 0 && lm.second.cputimeseg != 0){
          pMenST1ID0 = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        } else if (lm.second.ID == 1 && lm.second.cputimeseg != 0){
          pMenST1ID1 = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        }

      } else if (lm.second.type == "Task2"){
        if (lm.second.cputimeseg == 0){
          pMenST2OL = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        }else if (lm.second.ID == 0 && lm.second.cputimeseg != 0){
          pMenST2ID0 = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        }else if (lm.second.ID == 1 && lm.second.cputimeseg != 0){
          pMenST2ID1 = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        }else if (lm.second.ID == 2){
          pMenST2ID2 = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        }else if (lm.second.ID == 3){
          pMenST2ID3 = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        }else if (lm.second.ID == 4){
          pMenST2ID4 = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        }

      } else if (lm.second.type == "Task3"){
        if (lm.second.cputimeseg == 0){
          pMenST3OL = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        }else if (lm.second.ID == 0 && lm.second.cputimeseg != 0){
          pMenST3ID0 = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        }else if (lm.second.ID == 1 && lm.second.cputimeseg != 0){
          pMenST3ID1 = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        }else if (lm.second.ID == 2){
          pMenST3ID2 = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        }else if (lm.second.ID == 3){
          pMenST3ID3 = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        }

      } else if (lm.second.type == "Task4"){
        if (lm.second.cputimeseg == 0){
          pMenST4OL = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        }else if (lm.second.ID == 0 && lm.second.cputimeseg != 0){
          pMenST4ID0 = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        } else if (lm.second.ID == 1 && lm.second.cputimeseg != 0){
          pMenST4ID1 = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        }else if (lm.second.ID == 2){
          pMenST4ID2 = memoryVecSeg{lm.second.ID, lm.second.start, lm.second.total};
        }
      }
    };

    printVecMen1.push_back(pMenAT1);
    printVecMen2.push_back(pMenAT2);
    printVecMen3.push_back(pMenAT3);
    printVecMen4.push_back(pMenAT4);

    rowMen1.push_back(pMenST1OL);
    rowMen1.push_back(pMenST1ID0);
    rowMen1.push_back(pMenST1ID1);

    rowMen2.push_back(pMenST2OL);
    rowMen2.push_back(pMenST2ID0);
    rowMen2.push_back(pMenST2ID1);
    rowMen2.push_back(pMenST2ID2);
    rowMen2.push_back(pMenST2ID3);
    rowMen2.push_back(pMenST2ID4);

    rowMen3.push_back(pMenST3OL);
    rowMen3.push_back(pMenST3ID0);
    rowMen3.push_back(pMenST3ID1);
    rowMen3.push_back(pMenST3ID2);
    rowMen3.push_back(pMenST3ID3);

    rowMen4.push_back(pMenST4OL);
    rowMen4.push_back(pMenST4ID0);
    rowMen4.push_back(pMenST4ID1);
    rowMen4.push_back(pMenST4ID2);

    printVecSeg1.push_back(rowMen1);
    printVecSeg2.push_back(rowMen2);
    printVecSeg3.push_back(rowMen3);
    printVecSeg4.push_back(rowMen4);

  }
  saveMenToCSV("results/MenTask1.csv", printVecMen1);
  saveMenToCSV("results/MenTask2.csv", printVecMen2);
  saveMenToCSV("results/MenTask3.csv", printVecMen3);
  saveMenToCSV("results/MenTask4.csv", printVecMen4);

  saveSegmentToCSV("results/SegmentosTask1.csv", printVecSeg1);
  saveSegmentToCSV("results/SegmentosTask2.csv", printVecSeg2);
  saveSegmentToCSV("results/SegmentosTask3.csv", printVecSeg3);
  saveSegmentToCSV("results/SegmentosTask4.csv", printVecSeg4);

  saveCPUToCSV("results/Devices1.csv", printVecDisp1);
  saveCPUToCSV("results/Devices2.csv", printVecDisp2);
  saveCPUToCSV("results/Devices3.csv", printVecDisp3);
  saveCPUToCSV("results/Devices4.csv", printVecDisp4);

  saveCPUToCSV("results/CPUTask1.csv", printVecCPU1);
  saveCPUToCSV("results/CPUTask2.csv", printVecCPU2);
  saveCPUToCSV("results/CPUTask3.csv", printVecCPU3);
  saveCPUToCSV("results/CPUTask4.csv", printVecCPU4);

  saveCPUToCSV("results/Task1.csv", printVecTask1);
  saveCPUToCSV("results/Task2.csv", printVecTask2);
  saveCPUToCSV("results/Task3.csv", printVecTask3);
  saveCPUToCSV("results/Task4.csv", printVecTask4);

  return;
}

int main() {
  EventList eventlist = setup();
  eventlist.display();

  // Motor de eventos
  eventEngine(eventlist);
  cout << "Finalizacao da simulacao" << endl;
  return 0;
};