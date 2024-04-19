#ifndef umidita_h
#define umidita_h

#include <iostream>
#include <string>
#include <cmath>
#include "sensore.h"

class Umidità : public Sensore {
private:
    double temp;
public:
    Umidità(std::string, std::string, double, double);
    
    void setTemp(double);
    double getTemp() const;
    
    double val_Attuale() const;
    void control() const;
    QJsonObject sensor2Print() const;
};

#endif 
