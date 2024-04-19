#ifndef ADDSENSORE_H
#define ADDSENSORE_H

#include <QDialog>

class MainWindow;

namespace Ui {
class AddSensore;
}

class AddSensore : public QDialog
{
    Q_OBJECT

public:
    explicit AddSensore(MainWindow *parentMainWindow, QWidget *parent = nullptr);
    ~AddSensore();

private slots:
    void annullaButton_clicked();
    void confermaButton_clicked();
    void sensorType_textChanged();

private:
    Ui::AddSensore *ui;
    MainWindow *mainWindow;
};

#endif // ADDSENSORE_H
