#include <iostream>
#include <string>
#include "umidita.h"

Umidità::Umidità(std::string n, std::string s, double max, double temperatura) : Sensore(n,s,max), temp(temperatura) {}

void Umidità::setTemp(double a) {
    temp = a;
}

double Umidità::getTemp() const {
    return temp;
}

double Umidità::val_Attuale() const {
    double a = 8.071, b = 1730.630, c = 233.426;
    double logpS = a - (b/((temp+273.15)+c));
    double pS = pow(10, logpS);
    double pV = pS*0.6;
    return (pV/pS)*100;
}

void Umidità::control() const {
    if(this->val_Attuale() > getValMax()) std::cout << "umidità alta, attiva ventilazione";
}

QJsonObject Umidità::sensor2Print() const {
    QJsonObject sensore;
    sensore.insert("nome", QString::fromStdString(getName()));
    sensore.insert("tipo", QString::fromStdString(getType()));
    sensore.insert("temp_max", getValMax());
    sensore.insert("temperatura", temp);
    return sensore;
}
