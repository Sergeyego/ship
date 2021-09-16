#include "model.h"

ModelShip::ModelShip(QObject *parent) : DbTableModel("sertifikat",parent)
{
    addColumn("id",tr("id"),true,TYPE_INT);
    addColumn("nom_s",tr("Номер"),false,TYPE_STRING);
    addColumn("dat_vid",tr("Дата"),false,TYPE_DATE);
    addColumn("id_pol",tr("Получатель"),false,TYPE_STRING,NULL,Models::instance()->relPol);
    addColumn("id_type",tr("Тип отгрузки"),false,TYPE_STRING,NULL,Models::instance()->relShipType);
    setSort("sertifikat.dat_vid, sertifikat.nom_s");
    setDefaultValue(4,1);
}

void ModelShip::refresh(QDate beg, QDate end, int id_pol)
{
    QString filter="sertifikat.dat_vid between '"+beg.toString("yyyy-MM-dd")+"' and '"
            +end.toString("yyyy-MM-dd")+"'";
    if (id_pol!=-1){
        filter+=" and sertifikat.id_pol = "+QString::number(id_pol);
    }
    this->setFilter(filter);
    this->select();
}

bool ModelShip::insertRow(int row, const QModelIndex &parent)
{
    int old_num=0;
    if (rowCount()>0) old_num=this->data(this->index(rowCount()-1,1),Qt::EditRole).toInt();
    setDefaultValue(1,QString("%1").arg((old_num+1),4,'d',0,QChar('0')));
    setDefaultValue(2,QDate::currentDate());
    return DbTableModel::insertRow(row,parent);
}

ModelShipEl::ModelShipEl(QObject *parent) : DbTableModel("otpusk", parent)
{
    addColumn("id",tr("id"),true,TYPE_INT);
    addColumn("id_sert", tr("id_sert"),false,TYPE_INT);
    addColumn("id_part",tr("Партия"),false,TYPE_STRING,NULL,Models::instance()->relElPart);
    addColumn("massa",tr("Масса, кг"),false,TYPE_DOUBLE,new QDoubleValidator(0,999999999,3,this));
    setSort("otpusk.id");
    currentIdShip=-1;
    connect(this,SIGNAL(sigUpd()),this,SLOT(refreshState()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(refreshState()));
}

QVariant ModelShipEl::data(const QModelIndex &index, int role) const
{
    if((role == Qt::BackgroundColorRole)) {
        int area = colorState.value(DbTableModel::data(this->index(index.row(),2),Qt::EditRole).toInt());
        if(area == 4) return QVariant(QColor(255,170,170)); else
            if(area == 5) return QVariant(QColor(Qt::yellow)); else
                if(area == 6) return QVariant(QColor(Qt::gray)); else
                    if(area == 7) return QVariant(QColor(170,255,170)); else
                        return QVariant(QColor(255,200,100));
    } else return DbTableModel::data(index,role);
}

void ModelShipEl::refresh(int id_ship)
{
    currentIdShip=id_ship;
    setFilter("otpusk.id_sert = "+QString::number(id_ship));
    setDefaultValue(1,id_ship);
    select();
}

bool ModelShipEl::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ok=DbTableModel::setData(index,value,role);
    if (role==Qt::EditRole){
        emit sigStock(tr("Остаток на день отгрузки: ")+QString::number(getStock(index))+tr(" кг"));
    }
    return ok;
}

bool ModelShipEl::submitRow()
{
    bool ok = false;
    if (this->isEdt()){
        double kvo=this->data(this->index(currentEdtRow(),3),Qt::EditRole).toDouble();
        double m=getStock(this->index(currentEdtRow(),3));
        if (kvo>=0 && m>=kvo){
            ok=DbTableModel::submitRow();
        } else {
            QMessageBox::critical(NULL,tr("Ошибка"),tr("На складе на день отгрузки числится ")+
                                  QLocale().toString(m,'f',2)+tr(" кг электродов этой партии. Масса передачи должна быть положительной и не больше, чем числится на складе."),QMessageBox::Cancel);
        }
    }
    if (ok) emit sigStock("");
    return ok;
}

void ModelShipEl::escAdd()
{
    emit sigStock("");
    return DbTableModel::escAdd();
}

double ModelShipEl::getStock(QModelIndex index)
{
    double kvo=0;
    if (index.row()>=0 && index.row()<this->rowCount()){
        int id_part = this->data(this->index(index.row(),2),Qt::EditRole).toInt();
        int id_ship = this->data(this->index(index.row(),1),Qt::EditRole).toInt();
        QSqlQuery query;
        query.prepare("select kvoRs from calc_parti_one(:id_part, (select dat_vid from sertifikat where id = :id_ship ))");
        query.bindValue(":id_part",id_part);
        query.bindValue(":id_ship",id_ship);
        if (query.exec()){
            while (query.next()){
                kvo = query.value(0).toDouble();
            }
        } else {
            QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
        }
    }
    return kvo;
}

void ModelShipEl::refreshState()
{
    QSqlQuery query;
    query.setForwardOnly(true);
    query.prepare("select otpusk.id_part, "
                  "(select case when exists (select id_chem from sert_chem where id_part=otpusk.id_part) "
                  "then 1 else 0 end "
                  "+ "
                  "case when exists(select id_mech from sert_mech where id_part=otpusk.id_part) "
                  "then 2 else 0 end "
                  "+ "
                  "case when exists(select cod from td_keys_el where id_el=(select id_el from parti where id=otpusk.id_part) and id_diam=(select id from diam as d where d.diam=(select diam from parti where id=otpusk.id_part)) and id_pack=(select id_pack from parti where id=otpusk.id_part)) "
                  "then 4 else 0 end "
                  "as r) from otpusk where otpusk.id_sert = :id ");
    query.bindValue(":id",currentIdShip);
    if (query.exec()){
        colorState.clear();
        while (query.next()){
            colorState[query.value(0).toInt()]=query.value(1).toInt();
        }
        emit dataChanged(this->index(0,0),this->index(this->rowCount()-1,this->columnCount()-1));
    } else {
        QMessageBox::critical(NULL,tr("Error"),query.lastError().text(),QMessageBox::Ok);
    }
}

ModelShipWire::ModelShipWire(QObject *parent) : DbTableModel("wire_shipment_consist", parent)
{
    addColumn("id",tr("id"),true,TYPE_INT);
    addColumn("id_ship", tr("id_sert"),false,TYPE_INT);
    addColumn("id_wparti",tr("Партия"),false,TYPE_STRING,NULL,Models::instance()->relWirePart);
    addColumn("m_netto",tr("Масса, кг"),false,TYPE_DOUBLE,new QDoubleValidator(0,999999999,3,this));
    setSort("wire_shipment_consist.id");
    currentIdShip=-1;
    connect(this,SIGNAL(sigUpd()),this,SLOT(refreshState()));
    connect(this,SIGNAL(sigRefresh()),this,SLOT(refreshState()));
}

QVariant ModelShipWire::data(const QModelIndex &index, int role) const
{
    if((role == Qt::BackgroundColorRole)) {
        int area = colorState.value(DbTableModel::data(this->index(index.row(),2),Qt::EditRole).toInt());
        if(area == 4) return QVariant(QColor(255,170,170)); else
            if(area == 5) return QVariant(QColor(Qt::yellow)); else
                if(area == 6) return QVariant(QColor(Qt::gray)); else
                    if(area == 7) return QVariant(QColor(170,255,170)); else
                        return QVariant(QColor(255,200,100));
    } else return DbTableModel::data(index,role);
}

void ModelShipWire::refresh(int id_ship)
{
    currentIdShip=id_ship;
    setFilter("wire_shipment_consist.id_ship = "+QString::number(id_ship));
    setDefaultValue(1,id_ship);
    select();
}

bool ModelShipWire::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ok=DbTableModel::setData(index,value,role);
    if (role==Qt::EditRole){
        emit sigStock(tr("Остаток на день отгрузки: ")+QString::number(getStock(index))+tr(" кг"));
    }
    return ok;
}

bool ModelShipWire::submitRow()
{
    bool ok = false;
    if (this->isEdt()){
        double kvo=this->data(this->index(currentEdtRow(),3),Qt::EditRole).toDouble();
        double m=getStock(this->index(currentEdtRow(),3));
        if (kvo>=0 && m>=kvo){
            ok=DbTableModel::submitRow();
        } else {
            QMessageBox::critical(NULL,tr("Ошибка"),tr("На складе на день отгрузки числится ")+
                                  QLocale().toString(m,'f',2)+tr(" кг проволоки этой партии. Масса передачи должна быть положительной и не больше, чем числится на складе."),QMessageBox::Cancel);
        }
    }
    if (ok) emit sigStock("");
    return ok;
}

void ModelShipWire::escAdd()
{
    emit sigStock("");
    return DbTableModel::escAdd();
}

double ModelShipWire::getStock(QModelIndex index)
{
    double kvo=0;
    if (index.row()>=0 && index.row()<this->rowCount()){
        int id_part = this->data(this->index(index.row(),2),Qt::EditRole).toInt();
        int id_ship = this->data(this->index(index.row(),1),Qt::EditRole).toInt();
        QSqlQuery query;
        query.prepare("select st from wire_calc_stock((select dat_vid from sertifikat where id = :id_ship)) where id_wparti= :id_part");
        query.bindValue(":id_part",id_part);
        query.bindValue(":id_ship",id_ship);
        if (query.exec()){
            while (query.next()){
                kvo = query.value(0).toDouble();
            }
        } else {
            QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
        }
    }
    return kvo;
}

void ModelShipWire::refreshState()
{
    QSqlQuery query;
    query.setForwardOnly(true);
    query.prepare("select wire_shipment_consist.id_wparti, "
                  "(select case when exists(select id from wire_parti_chem "
                  "where id_part=(select p.id_m from wire_parti as p where p.id = wire_shipment_consist.id_wparti)) "
                  "then 1 else 0 end "
                  "+ "
                  "case when exists(select id from wire_parti_mech "
                  "where id_part=(select p.id_m from wire_parti as p where p.id = wire_shipment_consist.id_wparti)) "
                  "then 2 else 0 end "
                  "+ "
                  "case when exists(select cod from td_keys_wire "
                  "where id_prov=(select m.id_provol from wire_parti as wp inner join wire_parti_m as m on wp.id_m=m.id where wp.id=wire_shipment_consist.id_wparti) "
                  "and id_diam=(select m.id_diam from wire_parti as wp inner join wire_parti_m as m on wp.id_m=m.id where wp.id=wire_shipment_consist.id_wparti) "
                  "and id_spool=(select wp.id_pack from wire_parti as wp where wp.id=wire_shipment_consist.id_wparti) "
                  "and id_pack=(select wp.id_pack_type from wire_parti as wp where wp.id=wire_shipment_consist.id_wparti)) "
                  "then 4 else 0 end "
                  "as r) from wire_shipment_consist where wire_shipment_consist.id_ship = :id ");
    query.bindValue(":id",currentIdShip);
    if (query.exec()){
        colorState.clear();
        while (query.next()){
            colorState[query.value(0).toInt()]=query.value(1).toInt();
        }
        emit dataChanged(this->index(0,0),this->index(this->rowCount()-1,this->columnCount()-1));
    } else {
        QMessageBox::critical(NULL,tr("Error"),query.lastError().text(),QMessageBox::Ok);
    }
}

ModelNakl::ModelNakl(QObject *parent) : DbTableModel("prod_nakl",parent)
{
    addColumn("id",tr("id"),true,TYPE_INT);
    addColumn("num",tr("Номер"),false,TYPE_STRING);
    addColumn("dat",tr("Дата"),false,TYPE_DATE);
    addColumn("id_ist",tr("Тип"),false,TYPE_INT);
    setSort("prod_nakl.dat, prod_nakl.num");
}

void ModelNakl::refresh(QDate beg, QDate end, int id_type)
{
    QString filter="prod_nakl.dat between '"+beg.toString("yyyy-MM-dd")+"' and '"+end.toString("yyyy-MM-dd")+"' and prod_nakl.id_ist = "+QString::number(id_type);
    this->setDefaultValue(3,id_type);
    this->setFilter(filter);
    this->select();
}

bool ModelNakl::insertRow(int row, const QModelIndex &parent)
{
    int old_num=0;
    if (rowCount()>0) old_num=this->data(this->index(rowCount()-1,1),Qt::EditRole).toInt();
    if (defaultTmpRow.at(3)==1){
        setDefaultValue(1,QString::number(old_num+1));
    } else {
        setDefaultValue(1,QString("%1").arg((old_num+1),3,'d',0,QChar('0')));
    }
    setDefaultValue(2,QDate::currentDate());
    return DbTableModel::insertRow(row,parent);
}

ModelNaklCont::ModelNaklCont(QObject *parent) : DbTableModel("prod",parent)
{
    addColumn("id",tr("id"),true,TYPE_INT);
    addColumn("id_nakl",tr("id_nakl"),false,TYPE_INT);
    addColumn("docs",tr("Номер"),false,TYPE_STRING);
    addColumn("dat",tr("Дата"),false,TYPE_DATE);
    addColumn("id_ist",tr("Тип"),false,TYPE_INT);
    addColumn("id_part",tr("Партия"),false,TYPE_STRING,NULL,Models::instance()->relElPart);
    addColumn("kvo",tr("Масса, кг"),false,TYPE_DOUBLE,new QDoubleValidator(0,999999999,2,this));
    this->setSort("prod.id");
}

void ModelNaklCont::refresh(int id_nakl)
{
    QSqlQuery query;
    query.prepare("select id, num, dat, id_ist from prod_nakl where id= :id");
    query.bindValue(":id",id_nakl);
    if (query.exec()){
        while (query.next()){
            for (int i=0; i<query.record().count(); i++){
                defaultTmpRow[i+1]=query.value(i);
            }
        }
        this->setFilter("prod.id_nakl = "+QString::number(id_nakl));
        this->select();
    } else {
        this->setFilter("prod.id_nakl = -1");
        this->select();
        QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
    }
}

bool ModelNaklCont::setData(const QModelIndex &index, const QVariant &value, int role)
{
    bool ok=DbTableModel::setData(index,value,role);
    if (role==Qt::EditRole){
        emit sigStock(tr("Остаток на день передачи: ")+QString::number(getStock(index))+tr(" кг"));
    }
    return ok;
}

bool ModelNaklCont::submitRow()
{
    bool ok = false;
    if (this->isEdt() && defaultTmpRow.at(4)==11){
        double kvo=this->data(this->index(currentEdtRow(),6),Qt::EditRole).toDouble();
        double m=getStock(this->index(currentEdtRow(),6));
        if (kvo>=0 && m>=kvo){
            ok=DbTableModel::submitRow();
        } else {
            QMessageBox::critical(NULL,tr("Ошибка"),tr("На складе на день передачи числится ")+
                                  QLocale().toString(m,'f',2)+tr(" кг электродов этой партии. Масса передачи должна быть положительной и не больше, чем числится на складе."),QMessageBox::Cancel);
        }
    } else {
        ok=DbTableModel::submitRow();
    }
    if (ok) emit sigStock("");
    return ok;
}

void ModelNaklCont::escAdd()
{
    emit sigStock("");
    return DbTableModel::escAdd();
}

double ModelNaklCont::getStock(QModelIndex index)
{
    double kvo=0;
    if (index.row()>=0 && index.row()<this->rowCount()){
        int id_part = this->data(this->index(index.row(),5),Qt::EditRole).toInt();
        QDate date = this->data(this->index(index.row(),3),Qt::EditRole).toDate();
        QSqlQuery query;
        query.prepare("select kvoRs from calc_parti_one(:id_part, :date )");
        query.bindValue(":id_part",id_part);
        query.bindValue(":date",date);
        if (query.exec()){
            while (query.next()){
                kvo = query.value(0).toDouble();
            }
        } else {
            QMessageBox::critical(NULL,tr("Ошибка"),query.lastError().text(),QMessageBox::Ok);
        }
    }
    return kvo;
}
