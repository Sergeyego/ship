#ifndef FORMPRESENCEEL_H
#define FORMPRESENCEEL_H

#include <QWidget>
#include "modelpresence.h"
#include "modelreport.h"

namespace Ui {
class FormPresenceEl;
}

class FormPresenceEl : public QWidget
{
    Q_OBJECT

public:
    explicit FormPresenceEl(QWidget *parent = 0);
    ~FormPresenceEl();

private:
    Ui::FormPresenceEl *ui;
    ModelPresenceEl *modelPresEl;

private slots:
    void saveXls();
};

#endif // FORMPRESENCEEL_H
