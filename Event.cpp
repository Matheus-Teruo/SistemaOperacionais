#include "Event.h"
#include <string>

using namespace std;

Event::Event(int i, const string t, int f, int m, int c) : instant(i), type(t), flag(f), memory(m), cpuTime(c), next(nullptr) {};

int Event::getInstant() const{
    return instant;
};

string Event::getType() const{
    return type;
};

int Event::getFlag() const{
    return flag;
}

int Event::getMemory() const{
    return memory;
};

int Event::getcpuTime() const {
    return cpuTime;
};