#ifndef antincendio_h
#define antincendio_h

#include <iostream>
#include <string>
#include "sensore.h"

class Fumo : public Sensore {
private:
    double lvl_CO;
    // valMax corrisponde al nr. massimo di Ioni in percentuale (un valore compreso tra 0 e 1)
public:
    Fumo (std::string, std::string, double, double);
    
    void setLvlCo(double);
    double getLvlCo() const;
    double val_Attuale() const;
    void control() const;
    QJsonObject sensor2Print() const;
};

#endif
