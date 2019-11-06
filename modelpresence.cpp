#include "modelpresence.h"
#include <QMessageBox>

ModelPresence::ModelPresence(QObject *parent) :
    QSqlQueryModel(parent)
{
}

void ModelPresence::refresh(QDate date, bool bypart)
{
    this->clear();
    by_Part=bypart;
    if(by_Part){
        setQuery("select pr.nam, d.sdim, k.nam, m.n_s, cast(date_part('year',m.dat) as integer), s.nam, c.st from wire_parti p "
                 "inner join wire_parti_m as m on p.id_m=m.id "
                 "inner join provol pr on pr.id=m.id_provol "
                 "inner join (select cs.id_wparti, cs.st from wire_calc_stock('"+date.toString("yyyy.MM.dd")+"') cs) c on c.id_wparti=p.id "
                 "inner join diam d on d.id=m.id_diam "
                 "inner join wire_pack_kind k on p.id_pack=k.id "
                 "inner join wire_source s on m.id_source=s.id "
                 "where c.st <>0 "
                 "order by pr.nam, d.sdim, k.nam, m.n_s");
        if (lastError().isValid()){
            QMessageBox::critical(NULL,"Error",lastError().text(),QMessageBox::Cancel);
        } else {
            setHeaderData(0, Qt::Horizontal,tr("Марка"));
            setHeaderData(1, Qt::Horizontal,tr("Ф"));
            setHeaderData(2, Qt::Horizontal,tr("Катушка"));
            setHeaderData(3, Qt::Horizontal,tr("Партия"));
            setHeaderData(4, Qt::Horizontal,tr("Год"));
            setHeaderData(5, Qt::Horizontal,tr("Источник"));
            setHeaderData(6, Qt::Horizontal,tr("Кол-во, кг"));
            emit sigUpd();
        }
    } else {
        setQuery("select pr.nam, d.sdim, k.nam, sum(c.st) from wire_parti p "
                 "inner join wire_parti_m as m on p.id_m=m.id "
                 "inner join (select cs.id_wparti, cs.st from wire_calc_stock('"+date.toString("yyyy.MM.dd")+"') cs where cs.st<>0) c on c.id_wparti=p.id "
                 "inner join provol pr on pr.id=m.id_provol "
                 "inner join diam d on d.id=m.id_diam "
                 "inner join wire_pack_kind k on p.id_pack=k.id "
                 "inner join wire_source s on m.id_source=s.id "
                 "group by pr.nam, d.sdim, k.nam "
                 "order by pr.nam, d.sdim, k.nam");
        if (lastError().isValid()){
            QMessageBox::critical(NULL,"Error",lastError().text(),QMessageBox::Cancel);
        } else {
            setHeaderData(0, Qt::Horizontal,tr("Марка"));
            setHeaderData(1, Qt::Horizontal,tr("Ф"));
            setHeaderData(2, Qt::Horizontal,tr("Катушка"));
            setHeaderData(3, Qt::Horizontal,tr("Кол-во, кг"));
            emit sigUpd();
        }
    }
}

QVariant ModelPresence::data(const QModelIndex &index, int role) const
{   
    if (!index.isValid()) return QVariant();

    QVariant value;

    if ((index.row()==QSqlQueryModel::rowCount()) && (role==Qt::DisplayRole || role==Qt::EditRole)){
        if (index.column()==0) {
            value= tr("Итого");
        } else if ((index.column() == 6 && by_Part)||(index.column() == 3 && !by_Part)) {
            double sum=0.0;
            for (int i=0; i<QSqlQueryModel::rowCount(); i++){
                QModelIndex cs=this->index(i,index.column());
                sum+=(QSqlQueryModel::data(cs).toDouble());
            }
            value=sum;
        } else value=QVariant();

    } else {
        value = QSqlQueryModel::data(index, role);
    }

    switch (role) {
        case Qt::DisplayRole: // Данные для отображения
            if((index.column() == 6 && by_Part)||(index.column() == 3 && !by_Part)) return QLocale().toString(value.toDouble(),'f',2);
            else
                return value;

        case Qt::EditRole:    // Данные для редактирования
            return value;

        case Qt::TextAlignmentRole: // Выравнивание
            if((index.column() == 6 && by_Part)||(index.column() == 3 && !by_Part))
                return int(Qt::AlignRight | Qt::AlignVCenter);
            else return int(Qt::AlignLeft | Qt::AlignVCenter);
        case Qt::BackgroundColorRole:
        {
            double val= by_Part ? data(this->index(index.row(),6),Qt::EditRole).toDouble() :  data(this->index(index.row(),3),Qt::EditRole).toDouble();
            return val>=0 ? value : QVariant(QColor(255,170,170));
        }

    }
    return value;
}

int ModelPresence::rowCount(const QModelIndex &parent) const
{
    return (QSqlQueryModel::rowCount(parent)==0)? 0 : QSqlQueryModel::rowCount(parent)+1;
}


ModelPresenceEl::ModelPresenceEl(QObject *parent)
{
    bypart=false;
    sum=0.0;
}

QVariant ModelPresenceEl::data(const QModelIndex &item, int role) const
{
    if (!item.isValid()) return QVariant();
    if (item.row()==QSqlQueryModel::rowCount()){
        if (role==Qt::EditRole){
            if (item.column()==0) {
                return tr("Итого");
            } else if (item.column()==columnCount()-1) {
                return sum;
            } else {
                return QVariant();
            }
        } else if (role==Qt::DisplayRole){
            if (item.column()==0) {
                return tr("Итого");
            } else if (item.column()==columnCount()-1) {
                return QLocale().toString(sum,'f',1);
            } else {
                return QVariant();
            }
        } else if (role==Qt::TextAlignmentRole){
            if (item.column()==columnCount()-1) {
                return int(Qt::AlignRight | Qt::AlignVCenter);
            } else {
                return int(Qt::AlignLeft | Qt::AlignVCenter);
            }
        } else {
            return QVariant();
        }
    }

    QVariant val=QSqlQueryModel::data(item,Qt::EditRole);
    if (role==Qt::DisplayRole){
        if (val.typeName()==QString("double")){
            return QLocale().toString(val.toDouble(),'f',1);
        }
    }
    if (role==Qt::TextAlignmentRole){
        if (val.typeName()==QString("double") || val.typeName()==QString("int")){
            return int(Qt::AlignRight | Qt::AlignVCenter);
        }
    }
    return QSqlQueryModel::data(item,role);
}

int ModelPresenceEl::rowCount(const QModelIndex &parent) const
{
    return (QSqlQueryModel::rowCount(parent)==0)? 0 : QSqlQueryModel::rowCount(parent)+1;
}

void ModelPresenceEl::refresh(QDate date, bool by_part)
{
    this->clear();
    bypart=by_part;
    QSqlQuery query;
    if (bypart){
        query.prepare("select e.marka, p.diam, p.n_s, cast(date_part('year',p.dat_part) as integer), i.nam, n.nam, c.kvo "
                      "from calc_parti_new(:dat) as c "
                      "inner join parti as p on c.id_part=p.id "
                      "inner join elrtr as e on p.id_el=e.id "
                      "inner join istoch as i on p.id_ist=i.id "
                      "left join rcp_nam as n on p.id_rcp=n.id "
                      "where kvo<>0 order by e.marka, p.diam, p.n_s, p.dat_part");
    } else {
        query.prepare("select e.marka, p.diam, sum(c.kvo) "
                      "from calc_parti_new(:dat) as c "
                      "inner join parti as p on c.id_part=p.id "
                      "inner join elrtr as e on p.id_el=e.id "
                      "where kvo<>0 group by e.marka, p.diam order by e.marka, p.diam");
    }
    query.bindValue(":dat",date);
    if (query.exec()){
        sum=0.0;
        this->setQuery(query);
        int c = (bypart) ? 6 : 2;
        while (query.next()){
            sum += query.value(c).toDouble();
        }
        if (bypart){
            this->setHeaderData(0,Qt::Horizontal,tr("Марка"));
            this->setHeaderData(1,Qt::Horizontal,tr("Диам."));
            this->setHeaderData(2,Qt::Horizontal,tr("Партия"));
            this->setHeaderData(3,Qt::Horizontal,tr("Год"));
            this->setHeaderData(4,Qt::Horizontal,tr("Источник"));
            this->setHeaderData(5,Qt::Horizontal,tr("Рецептура"));
            this->setHeaderData(6,Qt::Horizontal,tr("Кол-во, кг"));
        } else {
            this->setHeaderData(0,Qt::Horizontal,tr("Марка"));
            this->setHeaderData(1,Qt::Horizontal,tr("Диам."));
            this->setHeaderData(2,Qt::Horizontal,tr("Кол-во, кг"));
        }
    } else {
        QMessageBox::critical(NULL,"Error",lastError().text(),QMessageBox::Cancel);
    }
}

