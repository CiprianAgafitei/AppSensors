#include <iostream>
#include <string>
#include "temperatura.h"

Temperatura::Temperatura(std::string n, std::string t, double att, double max, double min) :  Sensore(n,t,max), temp_att(att), temp_min(min) {}

void Temperatura::setTempAtt(double a) {
    temp_att = a;
}

double Temperatura::getTempMin() const {
    return temp_min;
}

void Temperatura::setTempMin(double a) {
    temp_min = a;
}

double Temperatura::val_Attuale() const {
    return temp_att;
}

void Temperatura::control() const {
    if(temp_att < temp_min) {std::cout << "temperatura bassa, attiva riscaldamento";}
    else if (temp_att > getValMax()) {std::cout << "temperatura alta, attiva climatizzazione";}
}

double Temperatura::tempKelvin() {
    return temp_att + 273.15;
}

QJsonObject Temperatura::sensor2Print() const {
    QJsonObject sensore;
    sensore.insert("nome", QString::fromStdString(getName()));
    sensore.insert("tipo", QString::fromStdString(getType()));
    sensore.insert("temp_max", getValMax());
    sensore.insert("temp_attuale", temp_att);
    sensore.insert("temp_minima", temp_min);
    return sensore;
}
