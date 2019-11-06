#ifndef PRESENCEWIDGET_H
#define PRESENCEWIDGET_H

#include <QWidget>
#include "modelpresence.h"

namespace Ui {
class PresenceWidget;
}

class PresenceWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit PresenceWidget(QWidget *parent = 0);
    ~PresenceWidget();
    
private:
    Ui::PresenceWidget *ui;
    ModelPresence *modelPresence;

private slots:
    void upd();
    void save();
};

#endif // PRESENCEWIDGET_H
