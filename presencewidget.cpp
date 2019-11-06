#include "presencewidget.h"
#include "ui_presencewidget.h"
#include <QCalendarWidget>

PresenceWidget::PresenceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PresenceWidget)
{
    ui->setupUi(this);
    ui->cmd_upd->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->cmd_save->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));
    ui->dateEdit->setDate(QDate::currentDate());
    modelPresence = new ModelPresence(this);
    ui->tableView->setModel(modelPresence);
    connect(ui->cmd_upd,SIGNAL(clicked()),this,SLOT(upd()));
    connect(ui->cmd_save,SIGNAL(clicked()),this,SLOT(save()));
}

PresenceWidget::~PresenceWidget()
{
    delete ui;
}

void PresenceWidget::upd()
{
    modelPresence->refresh(ui->dateEdit->date(),ui->radioButton_part->isChecked());
    ui->tableView->resizeToContents();
}

void PresenceWidget::save()
{
    ui->tableView->save(tr("Наличие проволоки на ")+ui->dateEdit->date().toString("dd.MM.yy"));
}

