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
    ui->tableView->setModel(modelPresEl);
    connect(ui->cmdUpd,SIGNAL(clicked(bool)),this,SLOT(updModel()));
    connect(ui->cmdSave,SIGNAL(clicked(bool)),this,SLOT(saveXls()));
}

FormPresenceEl::~FormPresenceEl()
{
    delete ui;
}

void FormPresenceEl::saveXls()
{
    ui->tableView->save(tr("Наличие электродов на ")+ui->dateEdit->date().toString("dd.MM.yy"));
}

void FormPresenceEl::updModel()
{
    modelPresEl->refresh(ui->dateEdit->date(),ui->radioButtonPart->isChecked());
    ui->tableView->resizeToContents();
}
