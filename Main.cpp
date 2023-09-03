#include <iostream>
#include <list>
#include "Event.h"

using namespace std;

list<Event> setup() {
  const Event event1(20, "Iniciar", 30, 60);
  const Event event2(20, "Task1", 100, 120);
  const Event event3(220, "Task2", 80, 80);
  const Event event4(240, "Finalizar", 40, 40);

  list<Event> EventList = {event1, event2, event3, event4};
  return EventList;
};

int main() {
  list<Event> eventList = setup();
  for (Event event : eventList) {
    const int text = event.getcpuTime();
    cout << text << endl;
  }
  return 0;
};