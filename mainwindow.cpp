#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(bool readonly, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadSettings();
    tabWidget = new TabWidget(this);
    this->setCentralWidget(tabWidget);
    formShip = new FormShip(readonly);
    formRet = new FormRet(readonly);
    formPartEl = new FormPartEl(readonly);
    formPartWire = new FormWarehouse();
    formPresWire = new PresenceWidget();
    formPresEl = new FormPresenceEl();
    formReport = new FormReport();

    tabWidget->addTabAction(formShip,ui->actionShip);
    tabWidget->addTabAction(formRet,ui->actionRet);
    tabWidget->addTabAction(formPartEl,ui->actionPartEl);
    tabWidget->addTabAction(formPartWire,ui->actionPartWire);
    tabWidget->addTabAction(formPresWire,ui->actionAvWire);
    tabWidget->addTabAction(formPresEl,ui->actionAvEl);
    tabWidget->addTabAction(formReport,ui->actionReportEl);

    tabWidget->loadSettings();

    connect(ui->actionExit,SIGNAL(triggered(bool)),this,SLOT(close()));
    connect(tabWidget,SIGNAL(currentChanged(int)),this,SLOT(setPartFilter()));
}

MainWindow::~MainWindow()
{
    saveSettings();
    tabWidget->saveSettings();
    delete formShip;
    delete formRet;
    delete formPartEl;
    delete formPartWire;
    delete formPresWire;
    delete formPresEl;
    delete formReport;
    delete ui;
}

void MainWindow::loadSettings()
{
    QSettings settings("szsm", "ship");
    this->restoreGeometry(settings.value("main_geometry").toByteArray());
    this->restoreState(settings.value("main_state").toByteArray());
}

void MainWindow::saveSettings()
{
    QSettings settings("szsm", "ship");
    settings.setValue("main_state", this->saveState());
    settings.setValue("main_geometry", this->saveGeometry());
}

void MainWindow::setPartFilter()
{
    if (tabWidget->currentWidget()==formShip){
        formShip->setPartFilter();
    } else if (tabWidget->currentWidget()==formRet){
        formRet->setPartFilter();
    }
}

