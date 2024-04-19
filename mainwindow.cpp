#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMessageBox>
#include <QFileDialog>
#include <QGraphicsTextItem>
#include <QTimer>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "temperatura.h"
#include "umidita.h"
#include "antincendio.h"
#include "clickableframe.h"
#include "editsensore.h"
#include "addsensore.h"
#include "simulationsettings.h"

QVector<Sensore*> *MainWindow::sensori = new QVector<Sensore*>();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    scrollLayout = new QVBoxLayout;
    setWindowTitle("AppSensors - Gestisci i tuoi sensori");
    QIcon icon(":/immagini/SensorsApp.jpg");
    setWindowIcon(icon);

    ui->warningText->setVisible(false);

    // Caricamento dei sensori da file
    currentFileName = "sensori.json";
    QFile file(currentFileName);
    load(file);

    if(sensori->size() == 0)
    {
        // Inserimento sensori di default per prova
        Temperatura* t1 = new Temperatura("Temp-01A1", "temperatura", 19.8, 28.5, 17.0);
        Temperatura* t2 = new Temperatura("Temp-02A3", "temperatura", 21.5, 21.0, 16.0);
        Temperatura* t3 = new Temperatura("Temp-07C9", "temperatura", 18.9, 23.0, 14.5);
        Temperatura* t4 = new Temperatura("Temp-05G1", "temperatura", 20.0, 25.5, 21.0);

        Umidità* u1 = new Umidità("Umid-1A43B", "umidità", 21.0, 20.1);
        Umidità* u2 = new Umidità("Umid-3C90F", "umidità", 18.5, 19.3);
        Umidità* u3 = new Umidità("Umid-2B11H", "umidità", 16.0, 14.7);

        Fumo* f1 = new Fumo("Fumo-012A", "fumo", 0.50, 55);
        Fumo* f2 = new Fumo("Fumo-237B", "fumo", 0.65, 22);

        sensori->append(t1);
        sensori->append(t2);
        sensori->append(t3);
        sensori->append(t4);
        sensori->append(u1);
        sensori->append(u2);
        sensori->append(u3);
        sensori->append(f1);
        sensori->append(f2);

        QFile file("sensori.json"); // Salvataggio dei sensori
        save(file);
        updateElencoSensori();
    }

    connect(ui->Salva, &QPushButton::clicked, this, &MainWindow::salva_clicked);
    connect(ui->Apri, &QPushButton::clicked, this, &MainWindow::apri_clicked);
    connect(ui->deleteSensore, &QPushButton::clicked, this, &MainWindow::deleteSensore_clicked);
    connect(ui->addSensore, &QPushButton::clicked, this, &MainWindow::addSensore_clicked);
    connect(ui->editSensore, &QPushButton::clicked, this, &MainWindow::editSensore_clicked);
    connect(ui->simulate, &QPushButton::clicked, this, &MainWindow::simulate_clicked);

    // Gestione imput sul campo di ricerca
    QObject::connect(ui->campoRicerca, &QLineEdit::textChanged, this, [&]() {
        QString searchString = ui->campoRicerca->text();
        int risultatiRicerca = 0;

        for (int i = 0; i < scrollLayout->count(); ++i)
        {
            ClickableFrame *itemWidget = qobject_cast<ClickableFrame*>(scrollLayout->itemAt(i)->widget());
            if (itemWidget) {
                // Controlla se il nome o il tipo del sensore corrisponde al testo di ricerca
                QLabel *nameLabel = itemWidget->findChild<QLabel*>("sensorName");
                QLabel *typeLabel = itemWidget->findChild<QLabel*>("sensorType");
                if (nameLabel && typeLabel) {
                    QString name = nameLabel->text();
                    QString type = typeLabel->text();
                    bool esito = (name.contains(searchString, Qt::CaseInsensitive) || type.contains(searchString, Qt::CaseInsensitive));
                    itemWidget->setVisible(esito);

                    if (esito)
                        risultatiRicerca++;
                }
            }
        }
        // Verifica numero risultati per singolare/plurale
        if (risultatiRicerca == 1)
            ui->EsitoRicerca->setText(QString::number(risultatiRicerca) + " risultato");
        else
            ui->EsitoRicerca->setText(QString::number(risultatiRicerca) + " risultati");
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * Salvataggio del file .json nella posizione scelta dall'utente, con il nome scelto da esso
 */
void MainWindow::salva_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Salva File"), QDir::homePath(), tr("JSON Files (*.json)"));

    // Controlla se l'utente ha selezionato un file
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        save(file);
    }
}

/**
 * Caricamento del file scelto dall'utente
 */
void MainWindow::apri_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Apri File"), QDir::homePath(), tr("JSON Files (*.json)"));

    // Controlla se l'utente ha selezionato un file
    if (!fileName.isEmpty()) {
        currentFileName = fileName; // Salvataggio del percorso del file corrente
        QFile file(fileName);
        load(file);
    }
}

/**
 * Restituisce il valore della posizione del settore selezionato
 * @return il valore intero della posizione
 */
int MainWindow::getSelectedSensorIndex() const {
    return selectedSensorIndex;
}

/**
 * Restituisce il percorso con il nome del file corrente
 * @return la stringa con il percorso del file
 */
QString MainWindow::getCurrenFileName() const {
    return currentFileName;
}

/**
 * Verifica se esiste un sensore con il nome indicato
 * @param name è il valore per cui verificare se esiste già un sensore con tale nome
 * @return true se esiste già; false altrimenti.
 */
bool MainWindow::checkExistingSensor(std::string name)
{
    for (int i = 0; i < sensori->size(); i++) {
        if (sensori->at(i)->getName() == name)
            return true;
    }
    return false;
}

/**
 * Visualizzazione della finestra per l'aggiunta di un sensore in modalità
 * modale, in modo che l'utente non possa interagire con la schermata principale
 */
void MainWindow::addSensore_clicked()
{
    AddSensore *addSensoreUi = new AddSensore(this);
    addSensoreUi->setModal(true);
    addSensoreUi->exec();
}

/**
 * Visualizzazione della finestra per la modifica del sensore selezionato,
 * in modalità modale
 */
void MainWindow::editSensore_clicked()
{
    EditSensore *editSensoreUi = new EditSensore(this);
    editSensoreUi->setModal(true);
    editSensoreUi->exec();
}

/**
 * Visualizzazione del messaggio di richiesta conferma per la rimozione del sensore selezionato.
 * Confermando, verranno aggiornati: vettore dei sensori, elenco (interfaccia) dei sensori e file
 */
void MainWindow::deleteSensore_clicked()
{
    // MESSAGGIO di richiesta conferma
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Avviso", "Sei sicuro di voler eliminare il sensore selezionato?", QMessageBox::Yes|QMessageBox::No);

    // Controllo scelta ed eliminazione del libro dalla tabella, se scelta positiva
    if (reply == QMessageBox::Yes)
    {
        // Rimozione dall'elenco dei sensori
        sensori->removeAt(selectedSensorIndex);

        // Salvataggio su file dell'elenco dei sensori aggiornati
        QFile file_attuale(currentFileName);
        save(file_attuale);

        updateElencoSensori();
        ui->warningText->setVisible(false);
    }
}

/**
 * Visualizzazione della schermata per la simulazione
 */
void MainWindow::simulate_clicked()
{
    SimulationSettings *simulation = new SimulationSettings(this);
    simulation->setModal(true);
    simulation->exec();
}

/**
 * Restituisce l'elenco attuale del vettore dei sensori
 * @return un vettore di sensori
 */
QVector<Sensore*> MainWindow::getSensori()
{
    return *sensori;
}

/**
 * Visualizzazione delle informazioni, sul pannello di destra, del sensore selezionato
 * @param i è l'indice del sensore selezionato nel vettore
 */
void MainWindow::visualizzaInfo()
{
    ui->warningText->setVisible(false);

    for (int i = 0; i < sensori->size(); ++i) {
        ClickableFrame* itemWidget = qobject_cast<ClickableFrame*>(scrollLayout->itemAt(i)->widget());
        if (itemWidget) {
            itemWidget->setStyleSheet("background-color: transparent;");  // Resetta il colore di sfondo
        }
    }

    if (selectedSensorIndex >= 0 && selectedSensorIndex < sensori->size())
    {
        ClickableFrame* selectedWidget = qobject_cast<ClickableFrame*>(scrollLayout->itemAt(selectedSensorIndex)->widget());
        if (selectedWidget) {
            selectedWidget->setStyleSheet("background-color: lightblue;");
        }
        Sensore* sensoreSelezionato = sensori->at(selectedSensorIndex);
        ui->TitoloSensore->setText(QString::fromStdString(sensoreSelezionato->getName()));
        ui->tipoSensore->setText("Tipo: " + QString::fromStdString(sensoreSelezionato->getType()));
        ui->ValoreMassimo->setText("Temp. massima: " + QString::number(sensoreSelezionato->getValMax()));

        // Attribuzione di un valore indefinito in modo che l'utente possa scegliere sia valori negativi, positivi o nulli
        double minTemp = std::numeric_limits<double>::quiet_NaN();
        double currentTemp = 0;

        // Impostato il valore diverso da tutti i sensori
        if (Temperatura* sensoreTemperatura = dynamic_cast<Temperatura*>(sensori->at(selectedSensorIndex))) {
            ui->campoComune->setText("Temp. Min: " + QString::number(sensoreTemperatura->getTempMin()));
            minTemp = sensoreTemperatura->getTempMin();
            currentTemp = sensoreTemperatura->val_Attuale();
        }
        else if (Umidità* sensoreUmidità = dynamic_cast<Umidità*>(sensori->at(selectedSensorIndex))) {
            ui->campoComune->setText("Temp.: " + QString::number(sensoreUmidità->getTemp()));
            currentTemp = sensoreUmidità->getTemp();
        }
        else if (Fumo* sensoreFumo = dynamic_cast<Fumo*>(sensori->at(selectedSensorIndex))) {
            ui->campoComune->setText("Livello CO: " + QString::number(sensoreFumo->getLvlCo()));
            ui->ValoreMassimo->setText("Nr. max di ioni: " + QString::number(sensoreFumo->getValMax()));
            currentTemp = sensoreFumo->getLvlCo() / 100;
        }

        // Creazione del grafico
        scene = new QGraphicsScene(this);
        ui->graphicsView->setScene(scene);

        double graphWidth = 180;
        double graphHeight = 180;
        scene->addLine(0, graphHeight, graphWidth, graphHeight);    // asse x
        scene->addLine(0, 0, 0, graphHeight);   // asse y

        // Disegna una retta parallela all'asse x per il valore massimo
        double maxTemp = sensori->at(selectedSensorIndex)->getValMax();
        double maxY = graphHeight - (maxTemp * (graphHeight / maxTemp));
        scene->addLine(0, maxY, graphWidth, maxY, QPen(Qt::red));
        QGraphicsTextItem *maxLabel = scene->addText(QString::fromStdString("Valore massimo"));
        maxLabel->setPos(graphWidth + 5, maxY - 10);

        QString yTextMax = QString::number(maxTemp) + "°C";
        QGraphicsTextItem *yLabel = scene->addText(yTextMax);
        yLabel->setPos(-40, maxY - 10);

        // Disegna una retta parallela all'asse x per il valore minimo della temperatura (SOLO per temperatura)
        if (!std::isnan(minTemp))
        {
            double minY = graphHeight - (minTemp * (graphHeight / maxTemp));
            scene->addLine(0, minY, graphWidth, minY, QPen(Qt::blue));
            QGraphicsTextItem *minLabel = scene->addText(QString::fromStdString("Valore minimo"));
            minLabel->setPos(graphWidth + 5, minY - 10);

            QString yTextMin = QString::number(minTemp) + "°C";
            QGraphicsTextItem *yLabel = scene->addText(yTextMin);
            yLabel->setPos(-40, minY - 10);
        }

        // Disegna un punto sul valore attuale a metà dell'asse x
        double currentTime = graphWidth / 2;
        double currentY = graphHeight - (currentTemp * (graphHeight / maxTemp));
        scene->addEllipse(currentTime - 2, currentY - 2, 4, 4, QPen(Qt::black), QBrush(Qt::black));
        QGraphicsTextItem *currentLabel = scene->addText(QString::fromStdString("Valore attuale"));
        currentLabel->setPos(currentTime + 5, currentY - 10);
    }
}

/**
 * Salvataggio su documento JSON dei sensori in base al tipo e al formato di ciascuno.
 */
void MainWindow::save(QFile &file)
{
    QJsonArray sensorsArray;

    for(int i = 0; i < sensori->size(); i++)
    {
        QJsonObject obj;

        Sensore *sensore = sensori->at(i);

        // Controllo tipo dinamico del sensore per inserimento su file con gli attributi di ciascuna categoria
        Temperatura *temperatura = dynamic_cast<Temperatura*>(sensore);
        Umidità *umidità = dynamic_cast<Umidità*>(sensore);
        Fumo *fumo = dynamic_cast<Fumo*>(sensore);

        // Inserimento dati sensore
        if(temperatura) {
            obj = temperatura->sensor2Print();
        }
        else if(umidità) {
            obj = umidità->sensor2Print();
        }
        else if(fumo) {
            obj = fumo->sensor2Print();
        }

        // Inserimento sensore nell'elenco dei sensori da scrivere sul file
        if(temperatura || umidità || fumo)
            sensorsArray.append(obj);
    }

    QJsonDocument document;
    document.setArray(sensorsArray);
    QByteArray bytes = document.toJson(QJsonDocument::Indented);
    file.open(QIODevice::WriteOnly | QIODevice::Text);

    // Salvataggio su file con messaggio di esito finale
    if (!file.isOpen() || !file.isWritable())
    {
        // Print error message
        QMessageBox msgBox;
        msgBox.setWindowTitle("Errore");
        msgBox.setText("Non è possibile aprire il file per salvare i sensori.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }
    else {
        // Controllo se vettore sensori vuoto
        if (sensori->size() == 0)
            file.resize(0);
        else
            file.write(bytes);
        file.flush();
        file.close();
    }
}

/**
 * Lettura e caricamento dei sensori da file JSON in un vettore ed
 * inserimento finale nell'interfaccia grafica.
 */
void MainWindow::load(QFile &file)
{
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        // Print error message
        QMessageBox msgBox;
        msgBox.setWindowTitle("Errore");
        msgBox.setText("Non è stato possibile aprire il file contenente i sensori.");
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.exec();
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();

    // file vuoto
    if(jsonData.isEmpty()) {
        return;
    }

    // Svuoto il vettore di sensori
    sensori->clear();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonArray jsonArray = doc.array();

    // Lettura e inserimento nel vettore dei sensori
    for (const auto &jsonValue : jsonArray)
    {
        QJsonObject jsonObject = jsonValue.toObject();

        QString type = jsonObject["tipo"].toString();

        // Controllo categoria del sensore
        if (type.toStdString() == "temperatura")
        {
            // creazione sensore di temperatura
            Temperatura *temperatura = new Temperatura(jsonObject["nome"].toString().toStdString(),
                                                       jsonObject["tipo"].toString().toStdString(),
                                                       jsonObject["temp_attuale"].toDouble(),
                                                       jsonObject["temp_max"].toDouble(),
                                                       jsonObject["temp_minima"].toDouble());
            sensori->push_back(temperatura);
        }
        else if (type.toStdString() == "umidità")
        {
            // creazione sensore di umidità
            Umidità *umidità = new Umidità(jsonObject["nome"].toString().toStdString(),
                                           jsonObject["tipo"].toString().toStdString(),
                                           jsonObject["temp_max"].toDouble(),
                                           jsonObject["temperatura"].toDouble());
            sensori->push_back(umidità);
        }
        else if (type.toStdString() == "fumo")
        {
            // creazione sensore di fumo
            Fumo *fumo = new Fumo(jsonObject["nome"].toString().toStdString(),
                                  jsonObject["tipo"].toString().toStdString(),
                                  jsonObject["max_ioni"].toDouble(),
                                  jsonObject["livelloCO"].toDouble());
            sensori->push_back(fumo);
        }
    }
    selectedSensorIndex = 0;
    updateElencoSensori();
}

/**
 * Aggiorna l'elenco dei sensori visibile con le informazioni presenti nel vettore sensori.
 */
void MainWindow::updateElencoSensori()
{
    QScrollArea *scrollArea = ui->elencoSensori;

    // Controllo se vettore dei sensori non vuoto
    if (sensori->size() > 0)
    {
        ui->warningText->setVisible(false);

        // Selezionato per default il primo elemento
        ui->TitoloSensore->setText(QString::fromStdString(sensori->at(0)->getName()));
        ui->tipoSensore->setText("Tipo: " + QString::fromStdString(sensori->at(0)->getType()));
        ui->ValoreMassimo->setText("Temp. massima: " + QString::number(sensori->at(0)->getValMax()));
        ui->EsitoRicerca->setText(QString::number(sensori->size()) + " risultati");
        ui->simulate->setVisible(true);
        ui->editSensore->setVisible(true);
        ui->deleteSensore->setVisible(true);
        ui->graphicsView->setVisible(true);
        ui->campoComune->setVisible(true);

        // Attribuzione di un valore indefinito in modo che l'utente possa scegliere sia valori negativi, positivi o nulli
        double minTemp = std::numeric_limits<double>::quiet_NaN();
        double currentTemp = 0;

        // Gestione del testo variabile di ciascun sensore
        if (Temperatura* sensoreTemperatura = dynamic_cast<Temperatura*>(sensori->at(0))) {
            ui->campoComune->setText("Temp. Min: " + QString::number(sensoreTemperatura->getTempMin()));
            minTemp = sensoreTemperatura->getTempMin();
            currentTemp = sensoreTemperatura->val_Attuale();
        }
        else if (Umidità* sensoreUmidità = dynamic_cast<Umidità*>(sensori->at(0))) {
            ui->campoComune->setText("Temp.: " + QString::number(sensoreUmidità->getTemp()));
            currentTemp = sensoreUmidità->val_Attuale();
        }
        else if (Fumo* sensoreFumo = dynamic_cast<Fumo*>(sensori->at(0))) {
            ui->campoComune->setText("Livello CO: " + QString::number(sensoreFumo->getLvlCo()));
            ui->ValoreMassimo->setText("Nr. massimo di ioni: " + QString::number(sensori->at(0)->getValMax()));
            currentTemp = sensoreFumo->val_Attuale() / 100;
        }

        // Creazione del grafico
        scene = new QGraphicsScene(this);
        ui->graphicsView->setScene(scene);

        double graphWidth = 180;
        double graphHeight = 180;
        scene->addLine(0, graphHeight, graphWidth, graphHeight);    // asse x
        scene->addLine(0, 0, 0, graphHeight);   // asse y

        // Disegna una retta parallela all'asse x per il valore massimo
        double maxTemp = sensori->at(0)->getValMax();
        double maxY = graphHeight - (maxTemp * (graphHeight / maxTemp));
        scene->addLine(0, maxY, graphWidth, maxY, QPen(Qt::red));
        QGraphicsTextItem *maxLabel = scene->addText(QString::fromStdString("Valore massimo"));
        maxLabel->setPos(graphWidth + 5, maxY - 10);

        QString yTextMax = QString::number(maxTemp) + "°C";
        QGraphicsTextItem *yLabel = scene->addText(yTextMax);
        yLabel->setPos(-40, maxY - 10);

        // Disegna una retta parallela all'asse x per il valore minimo della temperatura (SOLO in questo caso)
        if (!std::isnan(minTemp))
        {
            double minY = graphHeight - (minTemp * (graphHeight / maxTemp));
            scene->addLine(0, minY, graphWidth, minY, QPen(Qt::blue));
            QGraphicsTextItem *minLabel = scene->addText(QString::fromStdString("Valore minimo"));
            minLabel->setPos(graphWidth + 5, minY - 10);

            QString yTextMin = QString::number(minTemp) + "°C";
            QGraphicsTextItem *yLabel = scene->addText(yTextMin);
            yLabel->setPos(-40, minY - 10);
        }

        // Disegna un punto sul valore attuale a metà dell'asse x
        double currentTime = graphWidth / 2;
        double currentY = graphHeight - (currentTemp * (graphHeight / maxTemp));
        scene->addEllipse(currentTime - 2, currentY - 2, 4, 4, QPen(Qt::black), QBrush(Qt::black));
        QGraphicsTextItem *currentLabel = scene->addText(QString::fromStdString("Valore attuale"));
        currentLabel->setPos(currentTime + 5, currentY - 10);


        // Inserimento dei sensori come eleneco
        QWidget *scrollWidget = new QWidget(this);
        scrollLayout = new QVBoxLayout(scrollWidget);
        scrollLayout->setObjectName("sensorsList");

        QPixmap tempPixmap(":/immagini/temperatura.jpg");
        tempPixmap = tempPixmap.scaled(50, 50, Qt::KeepAspectRatio);
        QPixmap umidPixmap(":/immagini/umidity.jpg");
        umidPixmap = umidPixmap.scaled(50, 50, Qt::KeepAspectRatio);
        QPixmap fumoPixmap(":/immagini/antincendio.jpg");
        fumoPixmap = fumoPixmap.scaled(50, 50, Qt::KeepAspectRatio);

        for (int i = 0; i < sensori->size(); ++i) {
            ClickableFrame *itemWidget = new ClickableFrame(scrollWidget);
            itemWidget->setFrameStyle(QFrame::Box | QFrame::Plain); // Set the border style here
            itemWidget->setLineWidth(1);
            itemWidget->setMaximumHeight(70);   // altezza massima

            // Impostato il colore di sfondo (sul primo elemento viene messo effetto "selezionato")
            if (i == 0)
                itemWidget->setStyleSheet("background-color: lightblue;");
            else
                itemWidget->setStyleSheet("background-color: transparent;");

            QHBoxLayout *itemLayout = new QHBoxLayout(itemWidget);
            QLabel *imageLabel = new QLabel(itemWidget);
            imageLabel->setFixedSize(50, 50);

            if (sensori->at(i)->getType() == "temperatura")
                imageLabel->setPixmap(tempPixmap);
            else if (sensori->at(i)->getType() == "umidità")
                imageLabel->setPixmap(umidPixmap);
            else if (sensori->at(i)->getType() == "fumo")
                imageLabel->setPixmap(fumoPixmap);

            itemLayout->addWidget(imageLabel);
            itemLayout->addSpacing(10); // Inserimento di uno spazio tra l'immagine e le labels

            QVBoxLayout *textsLayout = new QVBoxLayout();
            QLabel *label1 = new QLabel(QString::fromStdString(sensori->at(i)->getName()), itemWidget);
            label1->setObjectName("sensorName");
            QLabel *label2 = new QLabel("Tipo: " + QString::fromStdString(sensori->at(i)->getType()), itemWidget);
            label2->setObjectName("sensorType");
            textsLayout->addWidget(label1);
            textsLayout->addWidget(label2);

            itemLayout->addLayout(textsLayout);
            scrollLayout->addWidget(itemWidget, i);

            // Collega il segnale del clic all'indice del vettore
            connect(itemWidget, &ClickableFrame::clicked, this, [=]() {
                itemWidget->setStyleSheet("background-color: gray;"); // Evidenzia il pulsante selezionato
                selectedSensorIndex = i;
                visualizzaInfo();
            });
        }
        scrollArea->setWidget(scrollWidget);
    }
    // Gestione caso in cui il vettore dei sensori è vuoto
    else {
        QWidget *scrollWidget = new QWidget(this);
        scrollArea->setWidget(scrollWidget);

        ui->TitoloSensore->setText(QString::fromStdString("Lista sensori vuota"));
        ui->tipoSensore->setText(QString::fromStdString("nessun sensore"));
        ui->ValoreMassimo->setText(QString::fromStdString(""));
        ui->EsitoRicerca->setText(QString::fromStdString(""));
        ui->simulate->setVisible(false);
        ui->editSensore->setVisible(false);
        ui->deleteSensore->setVisible(false);
        ui->graphicsView->setVisible(false);
        ui->campoComune->setVisible(false);
    }
}

/**
 * Avvio della simulazione dopo che l'utente ha impostato i valori desiderati e dopo
 * che è stato generato il vettore contenente i valori da inserire nel grafico, con effetto real-time
 * @param valori è il vettore con i valori da inserire nel grafico
 */
void MainWindow::simulazioneRealTime(QVector<double> valori)
{
    ui->warningText->setVisible(false);

    // Creazione del grafico
    scene = new QGraphicsScene(this);
    ui->graphicsView->setScene(scene);
    double graphWidth = 300;
    double graphHeight = 200;

    //  Inserimento valori ogni 15 minuti
    double intervalloTemporaleMinuti = 15;
    QTime orarioInizio(8, 0);  // Inizio dalle 08:00
    double xStep = graphWidth / (24 * 60 / intervalloTemporaleMinuti); // Calcola la dimensione dell'intervallo di tempo

    double tempoTotale = valori.size() * intervalloTemporaleMinuti;
    int larghezzaGrafico = xStep * tempoTotale;

    scene->addLine(0, graphHeight, larghezzaGrafico, graphHeight);    // asse x
    scene->addLine(0, 0, 0, graphHeight);   // asse y

    // Disegna una retta parallela all'asse x per il valore massimo
    double maxTemp = sensori->at(selectedSensorIndex)->getValMax();
    double maxY = graphHeight - (maxTemp * (graphHeight / maxTemp));
    scene->addLine(0, maxY, larghezzaGrafico, maxY, QPen(Qt::red));
    QGraphicsTextItem *maxLabel = scene->addText(QString::fromStdString("Valore massimo"));
    maxLabel->setPos(larghezzaGrafico + 5, maxY - 10);

    QString yTextMax = QString::number(maxTemp) + "°C";
    QGraphicsTextItem *yLabel = scene->addText(yTextMax);
    yLabel->setPos(-40, maxY - 10);

    // Disegna una retta parallela all'asse x per il valore minimo della temperatura (SOLO in questo caso)
    double minTemp = std::numeric_limits<double>::quiet_NaN();
    if (Temperatura* sensoreTemperatura = dynamic_cast<Temperatura*>(sensori->at(selectedSensorIndex))) {
        minTemp = sensoreTemperatura->getTempMin();
    }
    if (!std::isnan(minTemp)) {
        double minY = graphHeight - (minTemp * (graphHeight / maxTemp));
        scene->addLine(0, minY, larghezzaGrafico, minY, QPen(Qt::blue));
        QGraphicsTextItem *minLabel = scene->addText(QString::fromStdString("Valore minimo"));
        minLabel->setPos(larghezzaGrafico + 5, minY - 10);

        QString yTextMin = QString::number(minTemp) + "°C";
        QGraphicsTextItem *yLabel = scene->addText(yTextMin);
        yLabel->setPos(-40, minY - 10);
    }

    int currentIndex = 0; // Indice per tenere traccia dell'iterazione corrente
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=]() mutable {
        if (currentIndex < valori.size()) {
            double tempo = orarioInizio.msecsTo(orarioInizio.addSecs(currentIndex * 60 * intervalloTemporaleMinuti)) / 60000; // Calcola il tempo in minuti dall'inizio
            double temperatura = valori[currentIndex];
            double x = tempo * xStep;
            double y = graphHeight - (temperatura * (graphHeight / maxTemp));
            scene->addEllipse(x - 2, y - 2, 4, 4, QPen(Qt::black), QBrush(Qt::black));

            QTime orarioCorrente = orarioInizio.addSecs(currentIndex * 60 * intervalloTemporaleMinuti);
            QString xText = orarioCorrente.toString("HH:mm");
            QGraphicsTextItem *xLabel = scene->addText(xText);
            xLabel->setPos(x - 10, graphHeight+5);

            // Verifica se il valore attuale ha superato il limite previsto
            if (Temperatura* sensoreTemperatura = dynamic_cast<Temperatura*>(sensori->at(selectedSensorIndex))) {
                if (valori.at(currentIndex) > maxTemp) {
                    ui->warningText->setVisible(true);
                    ui->warningText->setText("Temperatura alta, attiva climatizzazione.");
                }
                else if (valori.at(currentIndex) < sensoreTemperatura->getTempMin()) {
                    ui->warningText->setVisible(true);
                    ui->warningText->setText("Temperatura bassa, attiva riscaldamento.");
                }
            }
            else if (dynamic_cast<Umidità*>(sensori->at(selectedSensorIndex))) {
                if (valori.at(currentIndex) > maxTemp) {
                    ui->warningText->setVisible(true);
                    ui->warningText->setText("Umidità alta, attiva ventilazione.");
                }
            }
            else if (dynamic_cast<Fumo*>(sensori->at(selectedSensorIndex))) {
                if (valori.at(currentIndex) > maxTemp) {
                    ui->warningText->setVisible(true);
                    ui->warningText->setText("Allarme antincendio attivato.");
                }
            }
            currentIndex++; // Incrementa l'indice per la prossima iterazione
            timer->start(1000); // Riavvia il timer per la prossima iterazione dopo 3 secondi
        }
    });
    // Avvia il timer per la prima iterazione dopo 3 secondi
    timer->start(1000); // Tempo in millisecondi (3 secondi)
}
