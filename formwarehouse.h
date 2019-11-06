#ifndef FORMWAREHOUSE_H
#define FORMWAREHOUSE_H

#include <QWidget>
#include "modelwarehouse.h"
#include <QSortFilterProxyModel>

namespace Ui {
class FormWarehouse;
}

class FormWarehouse : public QWidget
{
    Q_OBJECT
    
public:
    explicit FormWarehouse(QWidget *parent = 0);
    ~FormWarehouse();
    
private:
    Ui::FormWarehouse *ui;
    ModelWarehouse *modelWar;
    QSortFilterProxyModel *proxyModel;
    ModelCex *modelCex;
    ModelStock *modelStock;
    ModelShipment *modelShip;
    void loadSettings();
    void saveSettings();

private slots:
    void updData(QModelIndex index);

};

#endif // FORMWAREHOUSE_H
