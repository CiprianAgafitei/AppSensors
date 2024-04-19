#include <QMessageBox>
#include <random>
#include "simulationsettings.h"
#include "ui_simulationsettings.h"
#include "mainwindow.h"
#include "temperatura.h"
#include "umidita.h"
#include "antincendio.h"

SimulationSettings::SimulationSettings(MainWindow *parentMainWindow, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SimulationSettings),
    mainWindow(parentMainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Effettua una simulazione");
    QIcon icon(":/immagini/SensorsApp.jpg");
    setWindowIcon(icon);

    int indice = mainWindow->getSelectedSensorIndex();
    ui->tempMax->setText(QString::number(mainWindow->sensori->at(indice)->getValMax()));

    if (Temperatura* sensoreTemperatura = dynamic_cast<Temperatura*>(mainWindow->sensori->at(indice))) {
        ui->tempMinLabel->setVisible(true);
        ui->TempMin->setVisible(true);
        ui->TempMin->setText(QString::number(sensoreTemperatura->getTempMin()));
        ui->maxValueSet->setText("Temperatura massima impostata:");
    }
    else if (dynamic_cast<Umidità*>(mainWindow->sensori->at(indice))) {
        ui->tempMinLabel->setVisible(false);
        ui->TempMin->setVisible(false);
        ui->maxValueSet->setText("Temperatura massima impostata");
    }
    else if (dynamic_cast<Fumo*>(mainWindow->sensori->at(indice))) {
        ui->tempMinLabel->setVisible(false);
        ui->TempMin->setVisible(false);
        ui->maxValueSet->setText("Valore massimo di ioni(x/100):");
    }

    connect(ui->annullaButton, &QPushButton::clicked, this, &SimulationSettings::annullaButton_clicked);
    connect(ui->startButton, &QPushButton::clicked, this, &SimulationSettings::startButton_clicked);
}

SimulationSettings::~SimulationSettings()
{
    delete ui;
}

/**
 * Torna sulla pagina principale
 */
void SimulationSettings::annullaButton_clicked()
{
    this->hide();
}

/**
 * Genera un numero double random tra due estremi
 * @param min è l'estremo inferiore
 * @param max è l'estremo superiore
 * @return il numero double random generato
 */
double SimulationSettings::randomDouble(double min, double max) {
    std::random_device rd;  // Inizializza un generatore di numeri casuali
    std::mt19937 gen(rd()); // Utilizza un motore di numeri casuali Mersenne Twister con il seed della random_device

    std::uniform_real_distribution<double> distribution(min, max); // Distribuzione uniforme tra min e max

    return distribution(gen); // Genera e restituisce un numero double random compreso tra min e max
}

/**
 * Avvia la simulazione con i parametri indicati dall'utente in base al tipo di sensore selezionato
 */
void SimulationSettings::startButton_clicked()
{
    int indice = mainWindow->getSelectedSensorIndex();

    // Avvio simulazione
    QVector<double> valori;
    double valoreMinimo = ui->valMin->value();
    double valoreMassimo = ui->valMax->value();

    int nrValori = 29;

    if (Temperatura* sensoreTemperatura = dynamic_cast<Temperatura*>(mainWindow->sensori->at(indice)))
    {
        // Controllo valori massimo e minimo corretti
        if (ui->valMin->value() >= ui->valMax->value() || ui->valMin->value() + 0.9 >= ui->valMax->value()) {
            QMessageBox::information(this, "Attenzione", "Il valore scelto come minimo deve essere di almeno 1 grado inferiore alla temperatura massima.");
            return;
        }

        double valoreCorrente = randomDouble(valoreMinimo, valoreMassimo);
        valori.append(valoreCorrente);

        // Se il valore massimo > del limite massimo e il valore minimo > del limite minimo allora l'utente
        // vuole simulare la situazione di attivazione della climatizzazione
        if (valoreMassimo > sensoreTemperatura->getValMax() && valoreMinimo > sensoreTemperatura->getTempMin())
        {
            generaValoriRandomici(valori, 0.75, valoreCorrente, nrValori, 0.5, sensoreTemperatura);
        }
        // Se il valore massimo < del limite massimo e il valore minimo < del limite minimo allora l'utente
        // vuole simulare la situazione di attivazione del riscaldamento
        else if (valoreMassimo < sensoreTemperatura->getValMax() && valoreMinimo < sensoreTemperatura->getTempMin())
        {
            generaValoriRandomici(valori, 0.25, valoreCorrente, nrValori, 0.5, sensoreTemperatura);
        }
        // Altrimenti casualità pari
        else {
            generaValoriRandomici(valori, 0.5, valoreCorrente, nrValori, 0.5, sensoreTemperatura);
        }
    }
    else if (Umidità* sensoreUmidità = dynamic_cast<Umidità*>(mainWindow->sensori->at(indice)))
    {
        // Controllo valori massimo e minimo corretti
        if (ui->valMin->value() >= ui->valMax->value() || ui->valMin->value() + 0.9 >= ui->valMax->value()) {
            QMessageBox::information(this, "Attenzione", "Il valore scelto come minimo deve essere di almeno 1 grado inferiore alla temperatura massima.");
            return;
        }

        double valoreCorrente = randomDouble(valoreMinimo, valoreMassimo);
        valori.append(valoreCorrente);

        // Se valore massimo > del limite allora l'utente vuole simulare la situazione di attivazione della ventilazione
        // Allora la possibilità di crescere sarà maggiore rispetto a quella di scendere
        if (valoreMassimo > sensoreUmidità->getValMax())
        {
            generaValoriRandomici(valori, 0.75, valoreCorrente, nrValori, 0.5, sensoreUmidità);
        }
        else {
            generaValoriRandomici(valori, 0.5, valoreCorrente, nrValori, 0.5, sensoreUmidità);
        }
    }
    else if (Fumo* sensoreFumo = dynamic_cast<Fumo*>(mainWindow->sensori->at(indice)))
    {
        // Controllo valori massimo e minimo corretti
        if (ui->valMin->value() >= ui->valMax->value() || ui->valMin->value() + 0.09 >= ui->valMax->value()) {
            QMessageBox::information(this, "Attenzione", "Il valore scelto come minimo deve essere minore di almeno 0.1 rispetto al valore massimo.");
            return;
        }

        double valoreCorrente = randomDouble(valoreMinimo/100, valoreMassimo/100);
        valori.append(valoreCorrente);

        // Se valore massimo > del limite allora l'utente vuole simulare la situazione di incendio
        // allora la possibilità di crescere sarà maggiore rispetto a quella di scendere
        if (valoreMassimo/100 > sensoreFumo->getValMax())
        {
            generaValoriRandomici(valori, 0.75, valoreCorrente, nrValori, 0.05, sensoreFumo);
        }
        // Altrimenti generazione casuale alto/basso
        else {
            generaValoriRandomici(valori, 0.5, valoreCorrente, nrValori, 0.05, sensoreFumo);
        }
    }
    this->hide();
    mainWindow->simulazioneRealTime(valori);
}

/**
 * Generazione di massimo n valori a partire dal corrente e con probabilità prob di crescere o decrescere
 * a condizione che non si raggiunga la temperatura limite, altrimenti si ferma
 * Inoltre si è inserita anche la possibilità che la temperatura resti uguale alla precedente
 * @param valori è l'elenco dei valori in cui inserire i nuovi
 * @param probab è la probabilità per crescere
 * @param valCorr è il valore corrente di partenza
 * @param n è il numero di valori da generare
 * @param incremento è il valore da incrementare o decrementare
 * @param limite è il valore previsto per cui la generazione si fermerà perchè in stato di allarme
 */
void SimulationSettings::generaValoriRandomici(QVector<double>& valori, double probab, double valCorr, int n, double incremento, Sensore *s)
{
    for (int i = 0; i < n; ++i) {
        double probabilita = randomDouble(0.0, 1.0); // Genera una probabilità casuale tra 0 e 1

        // Si dà la possibilità anche che il valore non cresca
        int parteIntera = static_cast<int>(probabilita);
        if (probabilita * 10 - parteIntera != 0)
        {
            if (probabilita < probab) {
                valCorr += incremento;
            } else {
                valCorr -= incremento;
            }
        }
        valori.append(valCorr);

        // Controllo se valori limiti toccati
        if (valCorr > s->getValMax())
            return;
        if (Temperatura* sensoreTemperatura = dynamic_cast<Temperatura*>(s))
        {
            if (valCorr < sensoreTemperatura->getTempMin())
                return;
        }
    }
}
