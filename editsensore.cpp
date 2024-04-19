#include <QMessageBox>
#include "editsensore.h"
#include "ui_editsensore.h"
#include "mainwindow.h"
#include "temperatura.h"
#include "umidita.h"
#include "antincendio.h"

EditSensore::EditSensore(MainWindow *parentMainWindow, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditSensore),
    mainWindow(parentMainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Modifica dei parametri");
    QIcon icon(":/immagini/SensorsApp.jpg");
    setWindowIcon(icon);

    // Caricate le informazioni del sensore selezionato
    int indice = mainWindow->getSelectedSensorIndex();
    ui->sensorName->setText(QString::fromStdString(mainWindow->sensori->at(indice)->getName()));
    ui->sensorType->setText(QString::fromStdString(mainWindow->sensori->at(indice)->getType()));
    ui->sensorTempMax->setValue(mainWindow->sensori->at(indice)->getValMax());

    if (Temperatura* sensoreTemperatura = dynamic_cast<Temperatura*>(mainWindow->sensori->at(indice)))
    {
        ui->tempMinima->setVisible(true);
        ui->tempMinValue->setVisible(true);
        ui->tempMinValue->setValue(sensoreTemperatura->getTempMin());
        ui->sensorVar->setText(QString::fromStdString("Temperatura Attuale:"));
        ui->sensorVarValue->setValue(sensoreTemperatura->val_Attuale());
    }
    else if (Umidità* sensoreUmidità = dynamic_cast<Umidità*>(mainWindow->sensori->at(indice))) {
        ui->tempMinima->setVisible(false);
        ui->tempMinValue->setVisible(false);
        ui->sensorVar->setText(QString::fromStdString("Temperatura:"));
        ui->sensorVarValue->setValue(sensoreUmidità->getTemp());
    }
    else if (Fumo* sensoreFumo = dynamic_cast<Fumo*>(mainWindow->sensori->at(indice))) {
        ui->tempMinima->setVisible(false);
        ui->tempMinValue->setVisible(false);
        ui->sensorVar->setText(QString::fromStdString("Livello di CO:"));
        ui->sensorVarValue->setValue(sensoreFumo->getLvlCo());
        ui->maxVal->setText("Nr. max di ioni(/100):");
    }

    connect(ui->annullaModifica, &QPushButton::clicked, this, &EditSensore::annullaModifica_clicked);
    connect(ui->confermaModifica, &QPushButton::clicked, this, &EditSensore::confermaModifica_clicked);
}

EditSensore::~EditSensore()
{
    delete ui;
}

/**
 * Ritorno alla pagina principale
 */
void EditSensore::annullaModifica_clicked()
{
    this->hide();
}

/**
 * Conferma delle modifiche apportate al sensore selezionato sostituendo il vecchio con il nuovo
 */
void EditSensore::confermaModifica_clicked()
{
    // Controllo valori
    if (ui->tempMinValue->isVisible() && ui->tempMinValue->value() > ui->sensorTempMax->value()) {
        QMessageBox::information(this, "Attenzione", "Il valore scelto per la temperatura minima non può essere maggiore della temperatura massima.");
        return;
    }

    // Salvataggio delle modifiche
    int indice = mainWindow->getSelectedSensorIndex();
    mainWindow->sensori->at(indice)->setValMax(ui->sensorTempMax->value());

    // Sostituzione del sensore attuale con i valori aggiornati in base al tipo
    if (Temperatura* sensoreTemperatura = dynamic_cast<Temperatura*>(mainWindow->sensori->at(indice)))
    {
        sensoreTemperatura->setTempMin(ui->tempMinValue->value());
        sensoreTemperatura->setTempAtt(ui->sensorVarValue->value());
        mainWindow->sensori->replace(indice, sensoreTemperatura);
    }
    else if (Umidità* sensoreUmidità = dynamic_cast<Umidità*>(mainWindow->sensori->at(indice)))
    {
        sensoreUmidità->setTemp(ui->sensorVarValue->value());
        mainWindow->sensori->replace(indice, sensoreUmidità);
    }
    else if (Fumo* sensoreFumo = dynamic_cast<Fumo*>(mainWindow->sensori->at(indice)))
    {
        sensoreFumo->setLvlCo(ui->sensorVarValue->value());
        mainWindow->sensori->replace(indice, sensoreFumo);
    }
    this->hide();
    mainWindow->updateElencoSensori();
    QFile currentFile(mainWindow->getCurrenFileName());
    mainWindow->save(currentFile);
}
