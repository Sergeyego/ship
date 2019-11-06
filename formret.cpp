#include "formret.h"
#include "ui_formret.h"

FormRet::FormRet(bool readonly, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRet)
{
    ui->setupUi(this);
    ui->cmdUpd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->cmdUpdPart->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().dayOfYear()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    modelType = new DbRelationalModel("select id, nam from prod_nakl_tip where id in (1,4,5,11) order by nam",this);
    ui->comboBoxType->setModel(modelType);
    ui->comboBoxType->setModelColumn(1);

    ui->comboBoxPart->addItem(tr("начиная с текущего года"));
    ui->comboBoxPart->addItem(tr("начиная с прошлого года"));
    ui->comboBoxPart->addItem(tr("за всё время"));
    ui->comboBoxPart->setCurrentIndex(1);

    modelNaklCont = new ModelNaklCont(this);
    ui->tableViewCont->setModel(modelNaklCont);
    for (int i=0; i<5; i++){
        ui->tableViewCont->setColumnHidden(i,true);
    }
    ui->tableViewCont->setColumnWidth(5,350);
    ui->tableViewCont->setColumnWidth(6,100);

    modelNakl = new ModelNakl(this);
    ui->tableViewNakl->setModel(modelNakl);
    ui->tableViewNakl->setColumnHidden(0,true);
    ui->tableViewNakl->setColumnHidden(3,true);
    ui->tableViewNakl->setColumnWidth(1,55);
    ui->tableViewNakl->setColumnWidth(2,80);
    refreshNakl();

    push = new DbMapper(ui->tableViewNakl,this);
    ui->horizontalLayoutCmd->insertWidget(0,push);
    push->addMapping(ui->lineEditNum,1);
    push->addMapping(ui->dateEdit,2);
    push->addLock(ui->cmdUpd);
    push->addLock(ui->comboBoxType);
    push->addEmptyLock(ui->tableViewCont);

    if (readonly){
        ui->tableViewCont->setEditTriggers(QAbstractItemView::NoEditTriggers);
        push->setEnabled(false);
    }

    connect(ui->cmdUpd,SIGNAL(clicked(bool)),this,SLOT(refreshNakl()));
    connect(ui->comboBoxType,SIGNAL(currentIndexChanged(int)),this,SLOT(refreshNakl()));
    connect(push,SIGNAL(currentIndexChanged(int)),this,SLOT(setCurrentNakl(int)));
    connect(modelNaklCont,SIGNAL(sigStock(QString)),ui->labelStock,SLOT(setText(QString)));
    connect(ui->comboBoxPart,SIGNAL(currentIndexChanged(int)),this,SLOT(setPartFilter()));

    if (ui->tableViewNakl->model()->rowCount()){
        ui->tableViewNakl->selectRow(0);
    }
}

FormRet::~FormRet()
{
    delete ui;
}

void FormRet::refreshNakl()
{
    int id_type=ui->comboBoxType->model()->data(ui->comboBoxType->model()->index(ui->comboBoxType->currentIndex(),0),Qt::EditRole).toInt();
    modelNakl->refresh(ui->dateEditBeg->date(),ui->dateEditEnd->date(),id_type);
}

void FormRet::setCurrentNakl(int index)
{
    int id_nakl=ui->tableViewNakl->model()->data(ui->tableViewNakl->model()->index(index,0),Qt::EditRole).toInt();
    modelNaklCont->refresh(id_nakl);
}

void FormRet::setPartFilter()
{
    Models::instance()->setFilter(ui->comboBoxPart->currentIndex());
}
