#include "modelreadonly.h"

ModelPartEl::ModelPartEl(QObject *parent) : QSqlQueryModel(parent)
{

}

void ModelPartEl::refresh(QDate date, bool nn, bool cy, int id_el)
{
    QString flt;
    if (nn) flt+="c.kvo<>0 ";
    if (cy){
        if (!flt.isEmpty()) flt+="and ";
        flt+="date_part('year',p.dat_part) = "+QString::number(QDate::currentDate().year())+" ";
    }
    if (id_el!=-1){
        if (!flt.isEmpty()) flt+="and ";
        flt+="p.id_el = "+QString::number(id_el)+" ";
    }
    if (!flt.isEmpty()) flt="where "+flt;
    QSqlQuery query;
    query.prepare("select p.id, p.n_s||'-'||date_part('year',p.dat_part) as part, e.marka, p.diam, i.nam, c.kvo "
                  "from calc_parti_new(:date) as c "
                  "inner join parti as p on c.id_part=p.id "
                  "inner join elrtr as e on p.id_el=e.id "
                  "inner join istoch as i on p.id_ist=i.id "
                  +flt+"order by p.n_s, p.dat_part");
    query.bindValue(":date",date);
    if (!query.exec()){
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    } else {
        this->setQuery(query);
        this->setHeaderData(1,Qt::Horizontal,tr("Партия"));
        this->setHeaderData(2,Qt::Horizontal,tr("Марка"));
        this->setHeaderData(3,Qt::Horizontal,tr("Ф"));
        this->setHeaderData(4,Qt::Horizontal,tr("Источник"));
        this->setHeaderData(5,Qt::Horizontal,tr("Наличие, кг"));
    }
}

QVariant ModelPartEl::data(const QModelIndex &item, int role) const
{
    if (role==Qt::BackgroundColorRole){
        double val=QSqlQueryModel::data(index(item.row(),5),Qt::EditRole).toDouble();
        if (val<0.0) return QVariant(QColor(255,170,170));
        if (val>0.0) return QVariant(QColor(170,255,170));
    }
    if (item.column()==3 || item.column()==5){
        if (role==Qt::DisplayRole){
            int dec = item.column()==3 ? 1 : 2;
            return QLocale().toString(QSqlQueryModel::data(item,Qt::EditRole).toDouble(),'f',dec);
        } else if (role==Qt::TextAlignmentRole){
            return int(Qt::AlignRight | Qt::AlignVCenter);
        }
    }
    return QSqlQueryModel::data(item,role);
}

ModelPartElInfo::ModelPartElInfo(QObject *parent) : QSqlQueryModel(parent)
{

}

void ModelPartElInfo::refresh(int id_part, QDate date)
{
    QSqlQuery query;
    query.prepare("select c.sumIn, c.sumOt, c.kvoRs, p.fini, c.id_p from "
                  "calc_parti_one(:id_part, :date) as c "
                  "inner join parti as p on c.id_p=p.id");
    query.bindValue(":id_part",id_part);
    query.bindValue(":date",date);
    if (!query.exec()){
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    } else {
        this->setQuery(query);
    }
}

QVariant ModelPartElInfo::data(const QModelIndex &item, int role) const
{
    if (item.column()<3){
        if (role==Qt::DisplayRole || role==Qt::EditRole){
            return QLocale().toString(QSqlQueryModel::data(item,Qt::EditRole).toDouble(),'f',2);
        } else if (role==Qt::TextAlignmentRole){
            return int(Qt::AlignRight | Qt::AlignVCenter);
        }
    }
    return QSqlQueryModel::data(item,role);
}

bool ModelPartElInfo::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ok = false;
    if (index.column()==3){
        QSqlQuery query;
        query.prepare("update parti set fini = :val where id= :id");
        query.bindValue(":val",value.toBool());
        query.bindValue(":id",QSqlQueryModel::data(this->index(index.row(),4),Qt::EditRole).toInt());
        ok=query.exec();
        if (!ok){
            QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
        }
    }
    return ok;
}

Qt::ItemFlags ModelPartElInfo::flags(const QModelIndex &index) const
{
    if (index.column()==3){
        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsUserCheckable;
    }
    return QSqlQueryModel::flags(index);
}

ModelPartElPost::ModelPartElPost(QObject *parent) : QSqlQueryModel(parent)
{

}

void ModelPartElPost::refresh(int id_part)
{
    QSqlQuery query;
    query.prepare("select n.dat, n.num, i.nam, p.kvo "
                  "from prod as p "
                  "inner join prod_nakl as n on n.id=p.id_nakl "
                  "inner join istoch as i on i.id=n.id_ist "
                  "where p.id_part = :id "
                  "order by n.dat");
    query.bindValue(":id",id_part);
    if (!query.exec()){
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    } else {
        this->setQuery(query);
        this->setHeaderData(0,Qt::Horizontal,tr("Дата"));
        this->setHeaderData(1,Qt::Horizontal,tr("Накладная"));
        this->setHeaderData(2,Qt::Horizontal,tr("Источник"));
        this->setHeaderData(3,Qt::Horizontal,tr("Масса, кг"));
    }
}

QVariant ModelPartElPost::data(const QModelIndex &item, int role) const
{
    if (role==Qt::DisplayRole){
        if (item.column()==0) return QSqlQueryModel::data(item,Qt::EditRole).toDate().toString("dd.MM.yy");
        if (item.column()==3) return QLocale().toString(QSqlQueryModel::data(item,Qt::EditRole).toDouble(),'f',2);
    }
    if (role==Qt::TextAlignmentRole){
        return item.column()==3 ? int(Qt::AlignRight | Qt::AlignVCenter) : int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    return QSqlQueryModel::data(item,role);
}


ModelPartElShip::ModelPartElShip(QObject *parent)  : QSqlQueryModel(parent)
{

}

void ModelPartElShip::refresh(int id_part)
{
    QSqlQuery query;
    query.prepare("select s.nom_s, s.dat_vid, p.short, o.massa "
                  "from otpusk o inner join sertifikat s on o.id_sert=s.id "
                  "inner join poluch p on s.id_pol=p.id "
                  "where o.id_part = :id "
                  "order by s.dat_vid");
    query.bindValue(":id",id_part);
    if (!query.exec()){
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    } else {
        this->setQuery(query);
        this->setHeaderData(0,Qt::Horizontal,tr("Номер"));
        this->setHeaderData(1,Qt::Horizontal,tr("Дата"));
        this->setHeaderData(2,Qt::Horizontal,tr("Получатель"));
        this->setHeaderData(3,Qt::Horizontal,tr("Масса, кг"));
    }
}

QVariant ModelPartElShip::data(const QModelIndex &item, int role) const
{
    if (role==Qt::DisplayRole){
        if (item.column()==1) return QSqlQueryModel::data(item,Qt::EditRole).toDate().toString("dd.MM.yy");
        if (item.column()==3) return QLocale().toString(QSqlQueryModel::data(item,Qt::EditRole).toDouble(),'f',2);
    }
    if (role==Qt::TextAlignmentRole){
        return item.column()==3 ? int(Qt::AlignRight | Qt::AlignVCenter) : int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    return QSqlQueryModel::data(item,role);
}

ModelPartElOst::ModelPartElOst(QObject *parent) : QSqlQueryModel(parent)
{
    id_p=-1;
}

void ModelPartElOst::refresh(int id_part)
{
    QSqlQuery query;
    query.prepare("select id_part, dat, kvo, calc, corr "
                  "from ostat "
                  "where id_part = :id "
                  "order by dat");
    query.bindValue(":id",id_part);
    if (!query.exec()){
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    } else {
        id_p=id_part;
        this->setQuery(query);
        this->setHeaderData(1,Qt::Horizontal,tr("Дата"));
        this->setHeaderData(2,Qt::Horizontal,tr("Остаток, кг"));
        this->setHeaderData(3,Qt::Horizontal,tr("Расч. +, кг"));
        this->setHeaderData(4,Qt::Horizontal,tr("Корр., кг"));
    }
}

QVariant ModelPartElOst::data(const QModelIndex &item, int role) const
{
    if (role==Qt::DisplayRole){
        if (item.column()==1) return QSqlQueryModel::data(item,Qt::EditRole).toDate().toString("dd.MM.yy");
        if (item.column()>1) return QLocale().toString(QSqlQueryModel::data(item,Qt::EditRole).toDouble(),'f',2);
    }
    if (role==Qt::TextAlignmentRole){
        return item.column()>1 ? int(Qt::AlignRight | Qt::AlignVCenter) : int(Qt::AlignLeft | Qt::AlignVCenter);
    }
    return QSqlQueryModel::data(item,role);
}

bool ModelPartElOst::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ok = false;
    if (index.column()==2 && role==Qt::EditRole){
        QSqlQuery query;
        query.prepare("update ostat set kvo = :val where id_part= :id_part and dat = :dat ");
        query.bindValue(":val",value.toDouble());
        query.bindValue(":id_part",QSqlQueryModel::data(this->index(index.row(),0),Qt::EditRole).toInt());
        query.bindValue(":dat",QSqlQueryModel::data(this->index(index.row(),1),Qt::EditRole).toDate());
        ok=query.exec();
        if (!ok){
            QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
        }
    }
    if (index.column()==4 && role==Qt::EditRole){
        QSqlQuery query;
        query.prepare("update ostat set corr = :val where id_part= :id_part and dat = :dat ");
        query.bindValue(":val",value.toDouble());
        query.bindValue(":id_part",QSqlQueryModel::data(this->index(index.row(),0),Qt::EditRole).toInt());
        query.bindValue(":dat",QSqlQueryModel::data(this->index(index.row(),1),Qt::EditRole).toDate());
        ok=query.exec();
        if (!ok){
            QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
        }
    }
    if (ok){
        refresh(id_p);
    }
    return ok;
}

Qt::ItemFlags ModelPartElOst::flags(const QModelIndex &index) const
{
    if (index.column()==2 || index.column()==4){
        return Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    }
    return QSqlQueryModel::flags(index);
}
