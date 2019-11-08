#include "modelreport.h"

ModelPrg::ModelPrg(QObject *parent) :
    TableModel(parent)
{
    runner = new Executor(this,this);

    QSqlDatabase db=QSqlDatabase::database();
    databaseName=db.databaseName();
    hostName=db.hostName();
    port=db.port();
    userName=db.userName();
    password=db.password();
    connect(runner,SIGNAL(finished()),this,SLOT(refreshFinished()));
}

bool ModelPrg::loadData(const QString &query)
{
    bool ok=false;

    int randInt;
    QString randomName;
    for(int i=0; i<5; i++){
        randInt = qrand()%('Z'-'A'+1)+'A';
        randomName.append(randInt);
    }
    //qDebug() << randomName;
    {
        QSqlDatabase db=QSqlDatabase::addDatabase("QPSQL",randomName);
        db.setDatabaseName(databaseName);
        db.setHostName(hostName);
        db.setPort(port);
        db.setUserName(userName);
        db.setPassword(password);
        if (db.open()){
            QSqlQuery qu(db);
            qu.prepare(query);
            ok=qu.exec();
            if (ok){
                int n=qu.record().count();
                beginResetModel();
                modelData.clear();
                hHeader.resize(n);
                while(qu.next()){
                    typeRow t;
                    for (int j=0; j<n; j++){
                        t.push_back(qu.value(j));
                    }
                    modelData.push_back(t);
                }
                cCount=n;
                endResetModel();
            } else if (!ok) {
                setRowCount(0);
                emit sigError(qu.lastError().text());
            }
        } else {
            setRowCount(0);
            emit sigError(db.lastError().text());
        }
        if (db.isOpen()) db.close();
    }
    QSqlDatabase::removeDatabase(randomName);
    return ok;
}

void ModelPrg::run()
{
    runner->start();
}

void ModelPrg::refresh()
{

}

void ModelPrg::refreshFinished()
{
    this->layoutChanged();
    emit finished();
}

ModelReport::ModelReport(QObject *parent) :
    ModelPrg(parent)
{
    headerList<<tr("Марка")<<tr("ф")<<tr("Склад на\n нач. пер.")<<tr("План\n пр-ва")<<tr("Поступ.\n с пр-ва")<<tr("В т.ч. за\n посл.\n день");
    headerList<<tr("Поступ.\n от др.\n производ.")<<tr("Возвр.,\n переуп.\n (+/-)")<<tr("Неконд.")<<tr("Итого,\n поступл.");
    headerList<<tr("Отгруз. с\n нач. пер.")<<tr("В т.ч.за\n посл.\n день")<<tr("Остаток\n на кон.\n пер.");
}

QVariant ModelReport::data(const QModelIndex &index, int role) const
{
    if (role==Qt::DisplayRole && index.column()>1){
        double value=(ModelPrg::data(index, Qt::EditRole).toDouble())/1000.0;
        return (value!=0)? QLocale().toString(value,'f',4) : QString();
    }
    return ModelPrg::data(index, role);
}

QVariant ModelReport::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        return headerList.size()>section? headerList.at(section) : ModelPrg::headerData(section, orientation, role);
    }
    return ModelPrg::headerData(section, orientation, role);
}


void ModelReport::refresh()
{
    if (begDate.day()!=1) {
        emit sigError(tr("Начало периода должно быть первым числом месяца."));
        return;
    }

    QString query;
    query="select case when m.nk=false then m.el else "+tr("'нк '")+"|| m.el end, m.dim, m.ostls, m.plan, m.suma, m.sumtd, "
          "m.sumc, m.sumb, m.sume, m.sumin, m.sumot, m.sumjust, m.kvors "
          "from calc_marka_y_new(0,'"+begDate.toString("yyyy-MM-dd")+"','"+endDate.toString("yyyy-MM-dd")+"') as m "
          "inner join elrtr as e on m.id_el=e.id order by m.nk, e.id_u, m.el, m.dim";
    loadData(query);
    setRowCount(rowCount()+1);
    int row=rowCount()-1;
    QModelIndex ind=index(row,0);
    setData(ind,QString("Итого"),Qt::EditRole);
    for (int j=2; j<columnCount(); j++){
        double sum=0.0;
        for (int i=0; i<ModelPrg::rowCount(); i++){
            QModelIndex cs=index(i,j);
            sum+=ModelPrg::data(cs,Qt::EditRole).toDouble();
        }
        setData(index(row,j),sum,Qt::EditRole);
    }
}

void ModelReport::setEndDate(QDate date)
{
    endDate=date;
}

void ModelReport::setBegDate(QDate date)
{
    begDate=date;
}


ModelRest::ModelRest(QObject *parent) : ModelPrg(parent)
{

}

void ModelRest::refresh()
{
    if (date.month()!=date.addDays(1).month()){
        QString query="select * from calc_rest_new('"+date.toString("yyyy-MM-dd")+"')";
        loadData(query);
    } else {
        sigError(tr("Дата должна быть последним числом месяца."));
    }
}

void ModelRest::setDate(QDate d)
{
    date=d;
}
