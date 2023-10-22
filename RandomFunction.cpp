#include "RandomFunction.h"
#include <random>

std::random_device rd;
std::mt19937 gen(rd());

int randomBinary() {
    std::uniform_int_distribution<> dis(0, 1);
    if (dis(gen) == 0){ // granbiarra pra manter os exemplos
        return 2;
    } else {
        return 1;
    }
    return dis(gen);
};