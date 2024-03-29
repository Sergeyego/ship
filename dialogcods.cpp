#include "dialogcods.h"
#include "ui_dialogcods.h"

DialogCods::DialogCods(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogCods)
{
    ui->setupUi(this);
    relDiam = new DbRelation(new DbRelationalModel("select id, sdim from diam order by sdim",this),0,1,this);
    relElPack = new DbRelation(new DbRelationalModel("select id, pack_ed||'/'||pack_group from el_pack order by pack_ed",this),0,1,this);
    modelElCods = new DbTableModel("td_keys_el",this);
    modelElCods->addColumn("id_el",tr("Марка"),true,TYPE_STRING,NULL,Models::instance()->relEl);
    modelElCods->addColumn("id_diam",tr("Диаметр"),true,TYPE_STRING,NULL,relDiam);
    modelElCods->addColumn("id_pack",tr("Упаковка (ед./групп.)"),true,TYPE_STRING,NULL,relElPack);
    modelElCods->addColumn("cod",tr("Код"),false,TYPE_STRING);
    modelElCods->setSuffix("inner join elrtr on elrtr.id = td_keys_el.id_el inner join diam on diam.id = td_keys_el.id_diam");
    modelElCods->setSort("elrtr.marka, diam.sdim");
    modelElCods->select();
    ui->tableViewEl->setModel(modelElCods);
    ui->tableViewEl->setColumnWidth(0,180);
    ui->tableViewEl->setColumnWidth(1,100);
    ui->tableViewEl->setColumnWidth(2,210);
    ui->tableViewEl->setColumnWidth(3,100);

    relWire = new DbRelation(new DbRelationalModel("select id, nam from provol order by nam",this),0,1,this);
    relSpool = new DbRelation(new DbRelationalModel("select id, short from wire_pack_kind order by short",this),0,1,this);
    relPack = new DbRelation(new DbRelationalModel("select id, pack_ed as nam from wire_pack order by nam",this),0,1,this);
    modelWireCods = new DbTableModel("td_keys_wire",this);
    modelWireCods->addColumn("id_prov",tr("Марка"),true,TYPE_STRING,NULL,relWire);
    modelWireCods->addColumn("id_diam",tr("Диаметр"),true,TYPE_STRING,NULL,relDiam);
    modelWireCods->addColumn("id_spool",tr("Носитель"),true,TYPE_STRING,NULL,relSpool);
    modelWireCods->addColumn("id_pack",tr("Упаковка"),true,TYPE_STRING,NULL,relPack);
    modelWireCods->addColumn("cod",tr("Код"),false,TYPE_STRING);
    modelWireCods->setSuffix("inner join provol on provol.id = td_keys_wire.id_prov "
                             "inner join diam on diam.id = td_keys_wire.id_diam "
                             "inner join wire_pack_kind on wire_pack_kind.id = td_keys_wire.id_spool "
                             "inner join wire_pack on wire_pack.id = td_keys_wire.id_pack");
    modelWireCods->setSort("provol.nam, diam.sdim, wire_pack_kind.short, wire_pack.pack_ed");
    modelWireCods->select();
    ui->tableViewWire->setModel(modelWireCods);
    ui->tableViewWire->setColumnWidth(0,180);
    ui->tableViewWire->setColumnWidth(1,100);
    ui->tableViewWire->setColumnWidth(2,150);
    ui->tableViewWire->setColumnWidth(3,180);
    ui->tableViewWire->setColumnWidth(4,100);
}

DialogCods::~DialogCods()
{
    delete ui;
}
