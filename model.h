#ifndef MODEL_H
#define MODEL_H

#include <QObject>
#include "db/dbtablemodel.h"
#include <QMessageBox>
#include <QSqlRecord>
#include "models.h"

class ModelShip : public DbTableModel
{
    Q_OBJECT
public:
    ModelShip(QObject *parent=0);
    void refresh(QDate beg, QDate end, int id_pol=-1);
    bool insertRow(int row, const QModelIndex &parent = QModelIndex());

private:

};

class ModelShipEl : public DbTableModel
{
    Q_OBJECT
public:
    ModelShipEl(QObject *parent=0);
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    void refresh(int id_ship);
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool submitRow();
    void escAdd();
public slots:
    double getStock(QModelIndex index);
    void refreshState();
private:
    QMap <int,int> colorState;
    int currentIdShip;
signals:
    void sigStock(QString mes);
};

class ModelShipWire : public DbTableModel
{
    Q_OBJECT
public:
    ModelShipWire(QObject *parent=0);
    QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
    void refresh(int id_ship);
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool submitRow();
    void escAdd();
public slots:
    double getStock(QModelIndex index);
    void refreshState();
private:
    QMap <int,int> colorState;
    int currentIdShip;
signals:
    void sigStock(QString mes);
};

class ModelNakl : public DbTableModel
{
    Q_OBJECT
public:
    ModelNakl(QObject *parent=0);
    void refresh(QDate beg, QDate end, int id_type);
    bool insertRow(int row, const QModelIndex &parent = QModelIndex());
};

class ModelNaklCont : public DbTableModel
{
    Q_OBJECT
public:
    ModelNaklCont(QObject *parent=0);
    void refresh(int id_nakl);
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    bool submitRow();
    void escAdd();
public slots:
    double getStock(QModelIndex index);
signals:
    void sigStock(QString mes);
};


#endif // MODEL_H
