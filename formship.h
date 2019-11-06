#ifndef FORMSHIP_H
#define FORMSHIP_H

#include <QWidget>
#include "model.h"
#include "db/dbmapper.h"
#include "dialogcods.h"
#include <QDomDocument>
#include "models.h"

namespace Ui {
class FormShip;
}

class FormShip : public QWidget
{
    Q_OBJECT

public:
    explicit FormShip(bool readonly, QWidget *parent = 0);
    ~FormShip();

private:
    Ui::FormShip *ui;
    ModelShip *modelShip;
    DbMapper *push;
    ModelShipEl *modelShipEl;
    ModelShipWire *modelShipWire;
    void loadsettings();
    void savesettings();

public slots:
    void setPartFilter();

private slots:
    void updShip();
    void setCurrentShip(int index);
    void goXml();
    void edtCods();
    void updPol();
    QDomElement newElement(QString nam, QString val,  QDomDocument *doc);
};

#endif // FORMSHIP_H
