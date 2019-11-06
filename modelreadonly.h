#ifndef MODELREADONLY_H
#define MODELREADONLY_H

#include <QObject>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDate>
#include <QLocale>
#include "db/dbtablemodel.h"

class ModelPartEl : public QSqlQueryModel
{
    Q_OBJECT
public:
    ModelPartEl(QObject *parent=0);
    void refresh(QDate date, bool nn, bool cy, int id_el);
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
};

class ModelPartElInfo : public QSqlQueryModel
{
    Q_OBJECT
public:
    ModelPartElInfo(QObject *parent=0);
    void refresh(int id_part, QDate date);
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
};

class ModelPartElPost : public QSqlQueryModel
{
    Q_OBJECT
public:
    ModelPartElPost(QObject *parent=0);
    void refresh(int id_part);
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
};

class ModelPartElShip : public QSqlQueryModel
{
    Q_OBJECT
public:
    ModelPartElShip(QObject *parent=0);
    void refresh(int id_part);
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
};

class ModelPartElOst : public QSqlQueryModel
{
    Q_OBJECT
public:
    ModelPartElOst(QObject *parent=0);
    void refresh(int id_part);
    QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;
private:
    int id_p;
};


#endif // MODELREADONLY_H
