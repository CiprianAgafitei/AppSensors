#ifndef sensore_h
#define sensore_h

#include <iostream>
#include <string>
#include <QJsonObject>

class Sensore{
    friend class Temperatua;
    friend class Umidità;
    friend class Fumo;
private:
    std::string name, type;
    double valMax;
public:
    Sensore(std::string, std::string, double);
    virtual double val_Attuale() const = 0;
    virtual void control() const = 0;

    virtual QJsonObject sensor2Print() const = 0;

    std::string getName() const;
    void setName(std::string n);
    std::string getType() const;
    void setType(std::string t);
    double getValMax() const;
    void setValMax(double vmx);
};

#endif
