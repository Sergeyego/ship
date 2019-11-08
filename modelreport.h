#ifndef MODELREPORT_H
#define MODELREPORT_H

#include "tablemodel.h"
#include <QDate>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QStringList>
#include "executor.h"

class Executor;

class ModelPrg : public TableModel
{    Q_OBJECT
 public:
     explicit ModelPrg(QObject *parent = 0);
     bool loadData(const QString &query);
 public slots:
     virtual void refresh();
     virtual void refreshFinished();
     void run();
 signals:
     void sigError(QString);
     void finished();
 private:
      Executor *runner;
      QString databaseName;
      QString hostName;
      int port;
      QString userName;
      QString password;
};

class ModelReport : public ModelPrg
{
    Q_OBJECT
public:
    explicit ModelReport(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    
public slots:
    void refresh();
    void setBegDate(QDate date);
    void setEndDate(QDate date);

private:
    QDate begDate;
    QDate endDate;
    QStringList headerList;    
};

class ModelRest : public ModelPrg
{    Q_OBJECT
 public:
     explicit ModelRest(QObject *parent = 0);
 public slots:
     virtual void refresh();
     void setDate(QDate d);
 private:
     QDate date;
};

#endif // MODELREPORT_H
