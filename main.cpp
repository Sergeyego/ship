#include "mainwindow.h"
#include <QApplication>
#include "db/dblogin.h"

int main(int argc, char *argv[])
{
    QApplication a( argc, argv );
    a.setWindowIcon(QIcon(":/images/icon.ico"));

    QPixmap logo(":/images/simb_labl.png");
    DbLogin d(QObject::tr("Отгрузка продукции"),logo);
    if (d.exec()!=QDialog::Accepted) exit(1);

    MainWindow w(false);
    w.show();

    return a.exec();
}
