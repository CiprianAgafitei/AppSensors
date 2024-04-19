#ifndef SIMULATIONSETTINGS_H
#define SIMULATIONSETTINGS_H

#include <QDialog>
#include "sensore.h"

class MainWindow;

namespace Ui {
class SimulationSettings;
}

class SimulationSettings : public QDialog
{
    Q_OBJECT

public:
    explicit SimulationSettings(MainWindow *parentMainWindow, QWidget *parent = nullptr);
    ~SimulationSettings();

private slots:
    void annullaButton_clicked();
    void startButton_clicked();

private:
    Ui::SimulationSettings *ui;
    MainWindow *mainWindow;
    double randomDouble(double min, double max);
    void generaValoriRandomici(QVector<double>& valori, double probab, double valCorr, int n, double incremento, Sensore *s);
};

#endif // SIMULATIONSETTINGS_H
