#include "formreport.h"
#include "ui_formreport.h"

FormReport::FormReport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormReport)
{
    ui->setupUi(this);
    ui->cmdCalc->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_BrowserReload)));
    ui->cmdSave->setIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton)));
    ui->dateEditBeg->setDate(QDate::currentDate().addDays(-QDate::currentDate().day()+1));
    ui->dateEditEnd->setDate(QDate::currentDate());

    modelReport = new ModelReport(this);
    modelReport->setBegDate(ui->dateEditBeg->date());
    modelReport->setEndDate(ui->dateEditEnd->date());
    ui->tableViewRep->setModel(modelReport);

    connect(ui->cmdCalc,SIGNAL(clicked()),modelReport,SLOT(run()));
    connect(ui->dateEditBeg,SIGNAL(dateChanged(QDate)),modelReport,SLOT(setBegDate(QDate)));
    connect(ui->dateEditEnd,SIGNAL(dateChanged(QDate)),modelReport,SLOT(setEndDate(QDate)));
    connect(modelReport,SIGNAL(finished()),ui->tableViewRep,SLOT(resizeToContents()));
    connect(ui->cmdSave,SIGNAL(clicked()),this,SLOT(save()));
}

FormReport::~FormReport()
{
    delete ui;
}

void FormReport::save()
{
    int rows,cols;
    rows=ui->tableViewRep->model()->rowCount();
    cols=ui->tableViewRep->model()->columnCount();

    if (rows*cols>1){
        Document xlsx;
        Worksheet *ws=xlsx.currentWorksheet();

        XlsxPageSetup pageSetup;
        pageSetup.fitToPage=true;
        pageSetup.fitToWidth=1;
        pageSetup.fitToHeight=0;
        pageSetup.orientation=XlsxPageSetup::landscape;
        ws->setPageSetup(pageSetup);

        QFont defaultFont("Arial", 10);
        QFont titleFont("Arial", 10);
        titleFont.setBold(true);
        Format strFormat;
        strFormat.setBorderStyle(Format::BorderThin);
        strFormat.setFont(defaultFont);
        Format numFormat;
        numFormat.setBorderStyle(Format::BorderThin);
        numFormat.setFont(defaultFont);

        Format standardFormat;
        standardFormat.setBorderStyle(Format::BorderThin);
        standardFormat.setFont(defaultFont);

        Format headerFormat=strFormat;
        headerFormat.setFont(titleFont);
        headerFormat.setTextWarp(true);
        headerFormat.setHorizontalAlignment(Format::AlignHCenter);
        headerFormat.setVerticalAlignment(Format::AlignVCenter);

        Format edFormat=strFormat;
        edFormat.setHorizontalAlignment(Format::AlignHCenter);
        edFormat.setVerticalAlignment(Format::AlignVCenter);

        Format titleFormat;
        titleFormat.setFont(titleFont);

        QString fnam="Отчет по электродам ООО СЗСМ с "+ui->dateEditBeg->date().toString("dd.MM.yyyy")+" по "+ui->dateEditEnd->date().toString("dd.MM.yyyy");
        ws->writeString(CellReference("A1"),fnam,titleFormat);

        ws->setColumnWidth(1,1,20.9);
        ws->setColumnWidth(2,2,6.05);
        ws->setColumnWidth(3,13,10.5);

        ws->writeString(CellReference("A3"),QString("Марка"),headerFormat);
        ws->writeString(CellReference("B3"),QString("ф"),headerFormat);
        ws->writeString(CellReference("C3"),QString("Склад на начало месяца"),headerFormat);
        ws->writeString(CellReference("D3"),QString("План на месяц"),headerFormat);
        ws->writeString(CellReference("E3"),QString("Поступление на склад"),headerFormat);
        ws->writeString(CellReference("J3"),QString("Итого, поступление за месяц"),headerFormat);
        ws->writeString(CellReference("K3"),QString("Отгрузили"),headerFormat);
        ws->writeString(CellReference("M3"),QString("Остаток на складе на конец месяца"),headerFormat);
        ws->mergeCells(CellRange("E3:I3"),headerFormat);
        ws->mergeCells(CellRange("K3:L3"),headerFormat);

        ws->setRowHeight(4,4,44.6);

        ws->mergeCells(CellRange("A3:A4"),headerFormat);
        ws->mergeCells(CellRange("B3:B4"),headerFormat);
        ws->mergeCells(CellRange("C3:C4"),headerFormat);
        ws->mergeCells(CellRange("D3:D4"),headerFormat);
        ws->writeString(CellReference("E4"),QString("Производство"),headerFormat);
        ws->writeString(CellReference("F4"),QString("В т.ч. за последн. день"),headerFormat);
        ws->writeString(CellReference("G4"),QString("Поступл. от др. производ."),headerFormat);
        ws->writeString(CellReference("H4"),QString("Возврат на склад, переупаковка(+/-)"),headerFormat);
        ws->writeString(CellReference("I4"),QString("Некондиция"),headerFormat);
        ws->mergeCells(CellRange("J3:J4"),headerFormat);
        ws->writeString(CellReference("K4"),QString("С начала месяца"),headerFormat);
        ws->writeString(CellReference("L4"),QString("В т.ч. за последн. день"),headerFormat);
        ws->mergeCells(CellRange("M3:M4"),headerFormat);

        ws->writeBlank(CellReference("A5"),edFormat);
        ws->writeString(CellReference("B5"),QString("мм"),edFormat);
        for (int i=3; i<=13;i++){
             ws->writeString(5,i,QString("тонн"),edFormat);
        }

        int row=6;
        const int begRow=row;

        for (int i=0;i<rows-1;i++){
            for(int j=0;j<cols;j++){
                int role=Qt::EditRole;
                QVariant value=ui->tableViewRep->model()->data(ui->tableViewRep->model()->index(i,j),role);
                if (j==0){
                    ws->writeString(i+begRow,j+1,value.toString(),strFormat);
                } else if (j==1){
                    ws->writeNumeric(i+begRow,j+1,value.toDouble(),standardFormat);
                } else {
                    if (value.toDouble()!=0){
                        QString fmt=QString("0.%1").arg((0),4,'d',0,QChar('0'));
                        numFormat.setNumberFormat(fmt);
                        ws->writeNumeric(i+begRow,j+1,value.toDouble()/1000.0,numFormat);
                    } else {
                        QString fmt=QString("0.%1").arg((0),4,'d',0,QChar('0'));
                        numFormat.setNumberFormat(fmt);
                        ws->writeBlank(i+begRow,j+1,numFormat);
                    }
                }
            }
            row++;
        }

        ws->writeString(row,1,QString("Итого"),strFormat);
        ws->writeBlank(row,2,strFormat);
        QString fmt=QString("0.%1").arg((0),4,'d',0,QChar('0'));
        numFormat.setNumberFormat(fmt);
        ws->write(row,3,QString("=SUM(C%1:C%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,4,QString("=SUM(D%1:D%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,5,QString("=SUM(E%1:E%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,6,QString("=SUM(F%1:F%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,7,QString("=SUM(G%1:G%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,8,QString("=SUM(H%1:H%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,9,QString("=SUM(I%1:I%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,10,QString("=SUM(J%1:J%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,11,QString("=SUM(K%1:K%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,12,QString("=SUM(L%1:L%2)").arg(begRow).arg(row-1),numFormat);
        ws->write(row,13,QString("=SUM(M%1:M%2)").arg(begRow).arg(row-1),numFormat);

        QDir dir(QDir::homePath());
        QString filename = QFileDialog::getSaveFileName(nullptr,QString::fromUtf8("Сохранить документ"),
                                                        dir.path()+"/"+fnam+".xlsx",
                                                        QString::fromUtf8("Documents (*.xlsx)") );
        if (!filename.isEmpty()){
            xlsx.saveAs(filename);
        }
    }
}
