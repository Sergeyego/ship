#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "tabwidget.h"
#include "formship.h"
#include "formret.h"
#include "formpartel.h"
#include "formwarehouse.h"
#include "presencewidget.h"
#include "formpresenceel.h"
#include "formreport.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(bool readonly, QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    TabWidget *tabWidget;
    FormShip *formShip;
    FormRet *formRet;
    FormPartEl *formPartEl;
    FormWarehouse *formPartWire;
    PresenceWidget *formPresWire;
    FormPresenceEl *formPresEl;
    FormReport *formReport;
    void loadSettings();
    void saveSettings();

private slots:
    void setPartFilter();
};

#endif // MAINWINDOW_H
