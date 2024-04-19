#ifndef temperatura_h
#define temperatura_h

#include <iostream>
#include <string>
#include "sensore.h"

class Temperatura : public Sensore {
private:
    double temp_att, temp_min;
public:
    Temperatura(std::string, std::string, double, double, double);
    
    void setTempAtt(double);
    void setTempMin(double);
    double getTempMin() const;
    double val_Attuale () const;
    void control() const;
    double tempKelvin();
    QJsonObject sensor2Print() const;
};

#endif
