#ifndef MODELPRESENCE_H
#define MODELPRESENCE_H

#include <QSqlQueryModel>
#include <QtSql>

class ModelPresence : public QSqlQueryModel
{
    Q_OBJECT
public:
    explicit ModelPresence(QObject *parent = 0);
    void refresh(QDate date, bool bypart);
    QVariant data(const QModelIndex &index,int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    
private:
    bool by_Part;
    
public slots:

signals:
    void sigUpd();
    
};

class ModelPresenceEl : public QSqlQueryModel
{    Q_OBJECT
 public:
      explicit ModelPresenceEl(QObject *parent = 0);
      QVariant data(const QModelIndex &item, int role=Qt::DisplayRole) const;
      int rowCount(const QModelIndex &parent) const;
 public slots:
      void refresh(QDate date, bool by_part);
 private:
      bool bypart;
      double sum;
};

#endif // MODELPRESENCE_H
