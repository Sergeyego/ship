#include "formpartel.h"
#include "ui_formpartel.h"

FormPartEl::FormPartEl(bool readonly, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormPartEl)
{
    ui->setupUi(this);
    modelRest = new ModelRest(this);
    loadSettings();
    ui->dateEdit->setDate(QDate::currentDate());
    modelPartEl = new ModelPartEl(this);
    ui->tableViewPart->setModel(modelPartEl);
    ui->tableViewPart->verticalHeader()->hide();

    ui->comboBoxEl->setModel(Models::instance()->relEl->model());
    ui->comboBoxEl->setModelColumn(Models::instance()->relEl->columnDisplay());
    ui->comboBoxEl->completer()->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBoxEl->completer()->setCompletionMode(QCompleter::PopupCompletion);

    modelPartElInfo = new ModelPartElInfo(this);
    modelPartElInfo->refresh(-1,ui->dateEdit->date());
    mapperInfo = new QDataWidgetMapper(this);
    mapperInfo->setModel(modelPartElInfo);
    mapperInfo->addMapping(ui->lineEditPost,0);
    mapperInfo->addMapping(ui->lineEditOtp,1);
    mapperInfo->addMapping(ui->lineEditOst,2);
    mapperInfo->addMapping(ui->checkBoxFini,3);

    modelPartElPost = new ModelPartElPost(this);
    ui->tableViewPost->setModel(modelPartElPost);

    modelPartElShip = new ModelPartElShip(this);
    ui->tableViewShip->setModel(modelPartElShip);

    modelPartElOst = new ModelPartElOst(this);
    ui->tableViewOst->setModel(modelPartElOst);

    if (readonly){
        ui->tableViewOst->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->checkBoxFini->setEnabled(false);
        ui->cmdCalc->setEnabled(false);
    }

    setParams();

    connect(ui->cmdUpd,SIGNAL(clicked(bool)),modelPartEl,SLOT(run()));
    connect(modelPartEl,SIGNAL(finished()),this,SLOT(updPart()));
    connect(ui->checkBoxEl,SIGNAL(clicked(bool)),ui->comboBoxEl,SLOT(setEnabled(bool)));
    connect(ui->tableViewPart->selectionModel(),SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(updInfo(QModelIndex)));
    connect(ui->cmdCalc,SIGNAL(clicked(bool)),this,SLOT(calcOst()));
    connect(ui->dateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(setParams()));
    connect(ui->checkBoxNotN,SIGNAL(toggled(bool)),this,SLOT(setParams()));
    connect(ui->checkBoxCyrYear,SIGNAL(toggled(bool)),this,SLOT(setParams()));
    connect(ui->checkBoxEl,SIGNAL(toggled(bool)),this,SLOT(setParams()));
    connect(ui->comboBoxEl,SIGNAL(currentIndexChanged(int)),this,SLOT(setParams()));
}

FormPartEl::~FormPartEl()
{
    saveSettings();
    delete ui;
}

void FormPartEl::loadSettings()
{
    QSettings settings("szsm", "ship");
    this->ui->splitter->restoreState(settings.value("el_splitter_width").toByteArray());
}

void FormPartEl::saveSettings()
{
    QSettings settings("szsm", "ship");
    settings.setValue("el_splitter_width",ui->splitter->saveState());
}

void FormPartEl::updPart()
{
    /*int id_el=-1;
    if (ui->checkBoxEl->isChecked() && !ui->comboBoxEl->currentText().isEmpty()){
        id_el=ui->comboBoxEl->model()->data(ui->comboBoxEl->model()->index(ui->comboBoxEl->currentIndex(),0),Qt::EditRole).toInt();
    }
    modelPartEl->refresh(ui->dateEdit->date(), ui->checkBoxNotN->isChecked(), ui->checkBoxCyrYear->isChecked(),id_el);*/
    ui->tableViewPart->setColumnHidden(0,true);
    ui->tableViewPart->resizeToContents();
    ui->tableViewPart->selectRow(0);
    ui->tableViewPart->scrollToTop();
}

void FormPartEl::updInfo(QModelIndex index)
{
    int id_part=ui->tableViewPart->model()->data(ui->tableViewPart->model()->index(index.row(),0),Qt::EditRole).toInt();
    modelPartElInfo->refresh(id_part,ui->dateEdit->date());
    mapperInfo->toFirst();
    modelPartElPost->refresh(id_part);
    ui->tableViewPost->resizeToContents();
    modelPartElShip->refresh(id_part);
    ui->tableViewShip->resizeToContents();
    modelPartElOst->refresh(id_part);
    ui->tableViewOst->setColumnHidden(0,true);
    ui->tableViewOst->resizeToContents();
}

void FormPartEl::calcOst()
{
    DialogRest d;
    if (d.exec()==QDialog::Accepted){
        modelRest->setDate(d.getDate());
        modelRest->run();
    }
}

void FormPartEl::setParams()
{
    int id_el=-1;
    if (ui->checkBoxEl->isChecked() && !ui->comboBoxEl->currentText().isEmpty()){
        id_el=ui->comboBoxEl->model()->data(ui->comboBoxEl->model()->index(ui->comboBoxEl->currentIndex(),0),Qt::EditRole).toInt();
    }
    modelPartEl->setParams(ui->dateEdit->date(), ui->checkBoxNotN->isChecked(), ui->checkBoxCyrYear->isChecked(),id_el);
}
