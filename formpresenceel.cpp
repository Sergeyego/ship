#include "formpresenceel.h"
#include "ui_formpresenceel.h"

FormPresenceEl::FormPresenceEl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormPresenceEl)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->cmdUpd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->cmdSave->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));
    modelPresEl = new ModelPresenceEl(this);
    modelPresEl->setDate(ui->dateEdit->date());
    modelPresEl->setByPart(ui->radioButtonPart->isChecked());
    ui->tableView->setModel(modelPresEl);
    connect(ui->dateEdit,SIGNAL(dateChanged(QDate)),modelPresEl,SLOT(setDate(QDate)));
    connect(ui->radioButtonPart,SIGNAL(toggled(bool)),modelPresEl,SLOT(setByPart(bool)));
    connect(ui->cmdUpd,SIGNAL(clicked(bool)),modelPresEl,SLOT(run()));
    connect(modelPresEl,SIGNAL(finished()),ui->tableView,SLOT(resizeToContents()));
    connect(ui->cmdSave,SIGNAL(clicked(bool)),this,SLOT(saveXls()));
}

FormPresenceEl::~FormPresenceEl()
{
    delete ui;
}

void FormPresenceEl::saveXls()
{
    ui->tableView->save(tr("Наличие электродов на ")+ui->dateEdit->date().toString("dd.MM.yy"),1);
}
