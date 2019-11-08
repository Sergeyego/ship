#ifndef FORMREPORT_H
#define FORMREPORT_H

#include <QWidget>
#include "modelreport.h"
#include "xlsx/xlsxdocument.h"

using namespace QXlsx;

namespace Ui {
class FormReport;
}

class FormReport : public QWidget
{
    Q_OBJECT

public:
    explicit FormReport(QWidget *parent = 0);
    ~FormReport();

private:
    Ui::FormReport *ui;
    ModelReport *modelReport;

private slots:
    void save();
};

#endif // FORMREPORT_H
