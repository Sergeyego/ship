#include "executor.h"

Executor::Executor(ModelPrg *model, QObject *parent) :
    QThread(parent)
{
    report=model;
    pprd = new ProgressReportDialog();
    connect(this,SIGNAL(started()),pprd,SLOT(show()));
    connect(this,SIGNAL(finished()),pprd,SLOT(hide()));
    connect(report,SIGNAL(sigError(QString)),this,SLOT(showError(QString)));
}

Executor::~Executor()
{
    delete pprd;
}

void Executor::run()
{
    report->refresh();
}

void Executor::showError(QString text)
{
    QMessageBox::critical(NULL,tr("Ошибка"),text,QMessageBox::Ok);
}
