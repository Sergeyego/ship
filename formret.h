#ifndef FORMRET_H
#define FORMRET_H

#include <QWidget>
#include "db/dbtablemodel.h"
#include "model.h"
#include "db/dbmapper.h"

namespace Ui {
class FormRet;
}

class FormRet : public QWidget
{
    Q_OBJECT

public:
    explicit FormRet(bool readonly, QWidget *parent = 0);
    ~FormRet();

private:
    Ui::FormRet *ui;
    DbRelationalModel *modelType;
    ModelNakl *modelNakl;
    DbMapper *push;
    ModelNaklCont *modelNaklCont;

private slots:
    void refreshNakl();
    void setCurrentNakl(int index);

public slots:
    void setPartFilter();
};

#endif // FORMRET_H
