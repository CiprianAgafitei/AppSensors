#include <QMessageBox>
#include "ui_addsensore.h"
#include "addsensore.h"
#include "temperatura.h"
#include "umidita.h"
#include "antincendio.h"
#include "mainwindow.h"

AddSensore::AddSensore(MainWindow *parentMainWindow, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddSensore),
    mainWindow(parentMainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Aggiungi un nuovo sensore");
    QIcon icon(":/immagini/SensorsApp.jpg");
    setWindowIcon(icon);

    // Opzione inziale su tipo sensore: temperatura
    ui->textVar->setText("Temperatura Attuale:");

    connect(ui->annullaButton, &QPushButton::clicked, this, &AddSensore::annullaButton_clicked);
    connect(ui->confermaButton, &QPushButton::clicked, this, &AddSensore::confermaButton_clicked);
    connect(ui->sensorType, &QComboBox::currentTextChanged, this, &AddSensore::sensorType_textChanged);
}

AddSensore::~AddSensore()
{
    delete ui;
}

/**
 * Gestione dell'annullamento della creazione di un nuovo sensore.
 */
void AddSensore::annullaButton_clicked()
{
    this->hide();
}

/**
 * Gestione della conferma della creazione di un sensore con gestione dei vari errori possibili.
 */
void AddSensore::confermaButton_clicked()
{
    // Controllo campi non vuoti
    if (ui->sensorName->text().toStdString() == "") {
        QMessageBox::information(this, "Attenzione", "Il campo \"Nome\" è obbligatorio.");
        return;
    }
    // Controllo nome sensore già esistente
    else if (mainWindow->checkExistingSensor(ui->sensorName->text().toStdString())) {
        QMessageBox::information(this, "Attenzione", "Il nome scelto appartiene già ad un sensore esistente nell'elenco.");
        return;
    }

    // Controllo valore temperatura massima
    if (ui->sensorTempMax->value() <= 0) {
        QMessageBox::information(this, "Attenzione", "Il valore della temperatura massima scelto deve essere maggiore di 0.");
        return;
    }

    // Controllo tipo di sensore selezionato
    if (ui->sensorType->currentText() == "temperatura")
    {
        // Controllo se valore temperatura minima maggiore di quella massima
        if (ui->tempMinima->value() > ui->sensorTempMax->value()) {
            QMessageBox::information(this, "Attenzione", "Il valore della temperatura minima non può essere maggiore della temperatura massima.");
            return;
        }
        // Controllo valore della temperatura attuale
        if (ui->sensorVarValue->value() <= 0) {
            QMessageBox::information(this, "Attenzione", "Il valore della temperatura attuale scelto deve essere maggiore di 0.");
            return;
        }
        Temperatura *newSensore = new Temperatura(ui->sensorName->text().toStdString(), ui->sensorType->currentText().toStdString(), ui->sensorTempMax->value(), ui->sensorVarValue->value(), ui->tempMinima->value());
        MainWindow::sensori->push_back(newSensore);
    }
    else if (ui->sensorType->currentText() == "umidità")
    {
        // Controllo valore della temperatura
        if (ui->sensorVarValue->value() <= 0) {
            QMessageBox::information(this, "Attenzione", "Il valore della temperatura attuale scelto deve essere maggiore di 0.");
            return;
        }
        Umidità *newSensore = new Umidità(ui->sensorName->text().toStdString(), ui->sensorType->currentText().toStdString(), ui->sensorTempMax->value(), ui->sensorVarValue->value());
        MainWindow::sensori->push_back(newSensore);
    }
    else if (ui->sensorType->currentText() == "fumo")
    {
        // Controllo valore del livello di CO
        if (ui->sensorVarValue->value() < 0 || ui->sensorVarValue->value() > 100) {
            QMessageBox::information(this, "Attenzione", "Il livello di CO deve essere compreso tra 0 e 100.");
            return;
        }
        // Controllo valore massimo di ioni se impostato in notazione decimale o intera
        double maxIoni = ui->sensorTempMax->value();
        if (maxIoni > 1)
            maxIoni /= 100; // Trasforma in formato decimale
        Fumo *newSensore = new Fumo(ui->sensorName->text().toStdString(), ui->sensorType->currentText().toStdString(), maxIoni, ui->sensorVarValue->value());
        MainWindow::sensori->push_back(newSensore);
    }
    else {
        QMessageBox::information(this, "Attenzione", "Il campo \"Tipo di sensore\" è obbligatorio.");
        return;
    }
    this->hide();
    mainWindow->updateElencoSensori();
    QFile currentFile(mainWindow->getCurrenFileName());
    mainWindow->save(currentFile);
}

/**
 * Gestione in base al cambio dll'opzione relativa al tipo di temperatura
 */
void AddSensore::sensorType_textChanged()
{
    if (ui->sensorType->currentText() == "temperatura") {
        ui->tempMinLabel->setVisible(true);
        ui->tempMinima->setVisible(true);
        ui->textVar->setText("Temperatura Attuale:");
        ui->maxVal->setText("Temperatura massima:");
    }
    else if (ui->sensorType->currentText() == "umidità") {
        ui->tempMinLabel->setVisible(false);
        ui->tempMinima->setVisible(false);
        ui->textVar->setText("Temperatura:");
        ui->maxVal->setText("Temperatura massima:");
    }
    else if (ui->sensorType->currentText() == "fumo") {
        ui->tempMinLabel->setVisible(false);
        ui->tempMinima->setVisible(false);
        ui->textVar->setText("Livello di CO:");
        ui->maxVal->setText("Max. ioni(/100):");
    }
}
