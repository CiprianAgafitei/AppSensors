#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QVBoxLayout>
#include <QGraphicsScene>
#include "sensore.h"
#include "addsensore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static QVector<Sensore*> *sensori;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static void save(QFile &file); // Salvataggio su file
    void load(QFile &file); // Caricamento da file
    static QVector<Sensore*> getSensori();
    bool checkExistingSensor(std::string name);
    void updateElencoSensori();
    int getSelectedSensorIndex() const;
    QString getCurrenFileName() const;
    void simulazioneRealTime(QVector<double> valori);

private slots:
    void salva_clicked();
    void apri_clicked();
    void addSensore_clicked();
    void editSensore_clicked();
    void deleteSensore_clicked();
    void simulate_clicked();
    void visualizzaInfo();

private:
    Ui::MainWindow *ui;
    QVBoxLayout* scrollLayout;
    QString currentFileName;
    int selectedSensorIndex = -1;
    QGraphicsScene *scene;
};
#endif // MAINWINDOW_H
