#include <iostream>
#include <string>
#include "antincendio.h"

Fumo::Fumo(std::string n, std::string s, double max, double co) : Sensore(n,s,max), lvl_CO(co) {}

void Fumo::setLvlCo(double a){
    lvl_CO = a;
}

double Fumo::getLvlCo() const {
    return lvl_CO;
}

double Fumo::val_Attuale() const {
    return lvl_CO;
}

void Fumo::control() const {
    if (lvl_CO/100 > valMax) std::cout << "allarme antincendio attivata";
}

QJsonObject Fumo::sensor2Print() const {
    QJsonObject sensore;
    sensore.insert("nome", QString::fromStdString(getName()));
    sensore.insert("tipo", QString::fromStdString(getType()));
    sensore.insert("max_ioni", getValMax());
    sensore.insert("livelloCO", lvl_CO);
    return sensore;
}
