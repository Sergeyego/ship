#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <QThread>
#include "modelreport.h"
#include "progressreportdialog.h"
#include <QMessageBox>

class ModelPrg;

class Executor : public QThread
{
    Q_OBJECT
public:
    explicit Executor(ModelPrg *model, QObject *parent = 0);
    ~Executor();
    void run();
private:
    ModelPrg *report;
    ProgressReportDialog *pprd;
private slots:
    void showError(QString text);
};

#endif // EXECUTOR_H
