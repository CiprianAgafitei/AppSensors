#include <iostream>
#include <string>
#include "sensore.h"

Sensore::Sensore(std::string n, std::string t, double max) : name(n), type(t), valMax(max) {}

std::string Sensore::getName() const {
    return name;
}

void Sensore::setName(std::string n) {
    name = n;
}

std::string Sensore::getType() const {
    return type;
}

void Sensore::setType(std::string t) {
    type = t;
}

double Sensore::getValMax() const {
    return valMax;
}

void Sensore::setValMax(double vmx) {
    valMax = vmx;
}
