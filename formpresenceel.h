#ifndef FORMPRESENCEEL_H
#define FORMPRESENCEEL_H

#include <QWidget>
#include "modelpresence.h"

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
    void updModel();
};

#endif // FORMPRESENCEEL_H
