#include "modelwarehouse.h"
#include <QMessageBox>
#include <QDate>
#include <QLocale>

ModelWarehouse::ModelWarehouse(QObject *parent) :
    QSqlQueryModel(parent)
{
    notZero=false;
    curYear=true;
}

void ModelWarehouse::refresh()
{
    QString flt;
    if (notZero){
        flt+="s.st<>0 ";
    }
    if (curYear){
        if (!flt.isEmpty()) flt+="and ";
        QDate d(QDate::currentDate().year(),1,1);
        flt+="m.dat >= '"+d.toString("yyyy-MM-dd")+"' ";
    }
    if (!flt.isEmpty()){
        flt="where "+flt;
    }
    QString dat=date.toString("yyyy-MM-dd");
    setQuery("select s.id_wparti, m.n_s ||'-'|| date_part('year',m.dat), pr.nam, d.sdim, pk.short, s.st as sklad "
             "from wire_calc_stock('"+dat+"') as s "
             "inner join wire_parti as p on s.id_wparti=p.id "
             "inner join wire_parti_m as m on p.id_m=m.id "
             "inner join provol as pr on m.id_provol=pr.id "
             "inner join diam as d on m.id_diam=d.id "
             "inner join wire_pack_kind as pk on p.id_pack=pk.id "+
             flt+"order by m.dat, m.n_s");
    if (lastError().isValid()){
        QMessageBox::critical(NULL,"Error",lastError().text(),QMessageBox::Cancel);
    } else {
        setHeaderData(1, Qt::Horizontal,tr("Партия"));
        setHeaderData(2, Qt::Horizontal,tr("Марка"));
        setHeaderData(3, Qt::Horizontal,tr("Ф"));
        setHeaderData(4, Qt::Horizontal,tr("Намотка"));
        setHeaderData(5, Qt::Horizontal,tr("Наличие, кг"));
    }
    emit sigRefresh();
}

void ModelWarehouse::setNotZero(bool val)
{
    notZero=val;
    refresh();
}

void ModelWarehouse::setCurYear(bool val)
{
    curYear=val;
    refresh();
}

QVariant ModelWarehouse::data(const QModelIndex &item, int role) const
{
    if (role==Qt::BackgroundRole){
        double val1=QSqlQueryModel::data(index(item.row(),5),Qt::DisplayRole).toDouble();
        if (val1<0) return QVariant(QColor(255,170,170));
        if (val1>0) return QVariant(QColor(170,255,170));
    }
    if (item.column()==5){
        double val=QSqlQueryModel::data(item,Qt::DisplayRole).toDouble();
        if (role==Qt::TextAlignmentRole)
            return int(Qt::AlignRight | Qt::AlignVCenter);
        if (role==Qt::DisplayRole) return QLocale().toString(val,'f',2);
    }
    return QSqlQueryModel::data(item,role);
}

void ModelWarehouse::setDate(QDate d)
{
    date=d;
}


ModelCex::ModelCex(QObject *parent):
    QSqlQueryModel(parent)
{
    refresh(-1,QDate::currentDate());
}

QVariant ModelCex::data(const QModelIndex &item, int role) const
{
    if (item.column()==2){
        if (role==Qt::TextAlignmentRole) return int(Qt::AlignRight | Qt::AlignVCenter);
        if (role==Qt::DisplayRole) return QLocale().toString(QSqlQueryModel::data(item,role).toDouble(),'f',2);
    }
    if (item.column()==0 && role==Qt::DisplayRole)
        return QSqlQueryModel::data(item,role).toDate().toString("dd.MM.yy");
    return QSqlQueryModel::data(item,role);
}

void ModelCex::refresh(int id_part, QDate date)
{
    QString str_part=QString::number(id_part);
    QString str_date="'"+date.toString("yyyy-MM-dd")+"'";
    setQuery("(select d.dat, t.nam, d.m_netto from wire_in_cex_data as d "
             "inner join wire_in_cex_type as t on t.id=d.id_type "
             "where d.id_wparti= "+str_part+" and d.dat<= "+str_date+" ) "
             "union "
             "(select n.dat, "+tr("'Перепаковка в партию '") + " || m.n_s ||'-'||date_part('year',m.dat) as nam, p.m_netto from wire_perepack as p "
             "inner join wire_perepack_nakl as n on p.id_nakl=n.id "
             "inner join wire_parti as wp on p.id_wpartires=wp.id "
             "inner join wire_parti_m as m on wp.id_m=m.id "
             "where p.id_wpartisrc= "+str_part+" and n.dat<= "+str_date+" ) "
             "union "
             "(select n.dat, "+tr("'Перепаковка из парти '") + " || m.n_s ||'-'||date_part('year',m.dat) as nam, p.m_netto from wire_perepack as p "
             "inner join wire_perepack_nakl as n on p.id_nakl=n.id "
             "inner join wire_parti as wp on p.id_wpartisrc=wp.id "
             "inner join wire_parti_m as m on wp.id_m=m.id "
             "where p.id_wpartires= "+str_part+" and n.dat<= "+str_date+" ) "
             "order by dat, nam");
    if (lastError().isValid()){
        QMessageBox::critical(NULL,"Error",lastError().text(),QMessageBox::Cancel);
    } else {
        setHeaderData(0, Qt::Horizontal,tr("Дата"));
        setHeaderData(1, Qt::Horizontal,tr("Операция"));
        setHeaderData(2, Qt::Horizontal,tr("Масса, кг"));
    }
}


ModelStock::ModelStock(QObject *parent):
    QSqlQueryModel(parent)
{
    refresh(-1,QDate::currentDate());
}

QVariant ModelStock::data(const QModelIndex &item, int role) const
{
    if (role==Qt::BackgroundRole && QSqlQueryModel::data(index(item.row(),4),Qt::DisplayRole).toBool())
        return QVariant(QColor(255,200,100));
    if (item.column()==3){
        if (role==Qt::TextAlignmentRole) return int(Qt::AlignRight | Qt::AlignVCenter);
        if (role==Qt::DisplayRole) return QLocale().toString(QSqlQueryModel::data(item,role).toDouble(),'f',2);
    }
    if (item.column()==0 && role==Qt::DisplayRole)
        return QSqlQueryModel::data(item,role).toDate().toString("dd.MM.yy");
    return QSqlQueryModel::data(item,role);
}

void ModelStock::refresh(int id_part, QDate date)
{
    setQuery("select i.dat, i.num, t.nam, d.m_netto, i.nu from wire_warehouse as d "
             "inner join wire_whs_waybill as i on i.id=d.id_waybill "
             "inner join wire_way_bill_type as t on t.id=i.id_type "
             "where d.id_wparti="+QString::number(id_part)+" and i.dat<='"+date.toString("yyyy-MM-dd")+"' order by i.dat");
    if (lastError().isValid()){
        QMessageBox::critical(NULL,"Error",lastError().text(),QMessageBox::Cancel);
    } else {
        setHeaderData(0, Qt::Horizontal,tr("Дата"));
        setHeaderData(1, Qt::Horizontal,tr("Накладная"));
        setHeaderData(2, Qt::Horizontal,tr("Операция"));
        setHeaderData(3, Qt::Horizontal,tr("Масса, кг"));
    }
}


ModelShipment::ModelShipment(QObject *parent):
    QSqlQueryModel(parent)
{
    refresh(-1,QDate::currentDate());
}

QVariant ModelShipment::data(const QModelIndex &item, int role) const
{
    if (item.column()==4){
        if (role==Qt::TextAlignmentRole) return int(Qt::AlignRight | Qt::AlignVCenter);
        if (role==Qt::DisplayRole) return QLocale().toString(QSqlQueryModel::data(item,role).toDouble(),'f',2);
    }
    if (item.column()==0 && role==Qt::DisplayRole)
        return QSqlQueryModel::data(item,role).toDate().toString("dd.MM.yy");
    return QSqlQueryModel::data(item,role);
}

void ModelShipment::refresh(int id_part, QDate date)
{
    setQuery("select s.dat_vid, s.nom_s, p.short, t.nam, d.m_netto, d.id from wire_shipment_consist as d "
             "inner join sertifikat as s on s.id=d.id_ship "
             "inner join poluch as p on s.id_pol=p.id "
             "inner join sert_type as t on s.id_type=t.id "
             "where d.id_wparti="+QString::number(id_part)+" and s.dat_vid<='"+date.toString("yyyy-MM-dd")+"' order by s.dat_vid");
    if (lastError().isValid()){
        QMessageBox::critical(NULL,"Error",lastError().text(),QMessageBox::Cancel);
    } else {
        setHeaderData(0, Qt::Horizontal,tr("Дата"));
        setHeaderData(1, Qt::Horizontal,tr("№"));
        setHeaderData(2, Qt::Horizontal,tr("Получатель"));
        setHeaderData(3, Qt::Horizontal,tr("Тип отгрузки"));
        setHeaderData(4, Qt::Horizontal,tr("Масса, кг"));
    }
    double sum=0;
    for (int i=0; i<rowCount(); i++){
        sum+=data(index(i,3),Qt::EditRole).toDouble();
    }
    QString s;
    s = (sum>0)? (tr("Отгрузки итого: ")+QLocale().toString(sum,'f',2)+tr(" кг")) : tr("Отгрузки");
    emit sigSum(s);
}
