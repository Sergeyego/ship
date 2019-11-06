#ifndef FORMPARTEL_H
#define FORMPARTEL_H

#include <QWidget>
#include "modelreadonly.h"
#include "models.h"
#include <QDataWidgetMapper>

namespace Ui {
class FormPartEl;
}

class FormPartEl : public QWidget
{
    Q_OBJECT

public:
    explicit FormPartEl(bool readonly, QWidget *parent = 0);
    ~FormPartEl();

private:
    Ui::FormPartEl *ui;
    ModelPartEl *modelPartEl;
    ModelPartElInfo *modelPartElInfo;
    QDataWidgetMapper *mapperInfo;
    ModelPartElPost *modelPartElPost;
    ModelPartElShip *modelPartElShip;
    ModelPartElOst *modelPartElOst;
    void loadSettings();
    void saveSettings();

private slots:
    void updPart();
    void updInfo(QModelIndex index);
};

#endif // FORMPARTEL_H
