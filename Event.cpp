#include "Event.h"
#include <string>

using namespace std;

Event::Event( int i, const string t, int m, int c) : instant(i), type(t), memory(m), cpuTime(c) {};

int Event::getInstant() const{
    return instant;
};

string Event::getType() const{
    return type;
};

int Event::getMemory() const{
    return memory;
};

int Event::getcpuTime() const {
    return cpuTime;
};