#ifndef EDITSENSORE_H
#define EDITSENSORE_H

#include <QDialog>

class MainWindow;

namespace Ui {
class EditSensore;
}

class EditSensore : public QDialog
{
    Q_OBJECT

public:
    explicit EditSensore(MainWindow *parentMainWindow, QWidget *parent = nullptr);
    ~EditSensore();

private slots:
    void annullaModifica_clicked();
    void confermaModifica_clicked();

private:
    Ui::EditSensore *ui;
    MainWindow *mainWindow;
};

#endif // EDITSENSORE_H
