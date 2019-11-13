#ifndef MODELWAREHOUSE_H
#define MODELWAREHOUSE_H

#include <QSqlQueryModel>
#include <QSqlError>
#include <QDate>
#include "db/dbtablemodel.h"
#include "models.h"

class ModelWarehouse : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit ModelWarehouse(QObject *parent = 0);
public slots:
    void refresh();
    void setNotZero(bool val);
    void setCurYear(bool val);
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
    void setDate(QDate d);
private:
    bool notZero;
    bool curYear;
    QDate date;
    
signals:
    void sigRefresh();
public slots:
    
};

class ModelCex : public QSqlQueryModel
{
    Q_OBJECT
public:
    ModelCex(QObject *parent=0);
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
public slots:
    void refresh(int id_part, QDate date);
};

class ModelStock : public QSqlQueryModel
{
    Q_OBJECT
public:
    ModelStock(QObject *parent=0);
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
public slots:
    void refresh(int id_part, QDate date);
};

class ModelShipment : public QSqlQueryModel
{
    Q_OBJECT
public:
    ModelShipment(QObject *parent=0);
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
public slots:
    void refresh(int id_part, QDate date);
signals:
    void sigSum(QString sum);
};

#endif // MODELWAREHOUSE_H
