#include "tableview.h"

TableView::TableView(QWidget *parent) : QTableView(parent)
{
    verticalHeader()->setDefaultSectionSize(verticalHeader()->fontMetrics().height()*1.5);
}

void TableView::resizeToContents()
{
    if (!model()) return;
    int n=model()->columnCount();
    int m=model()->rowCount();
    int max=0;
    QStringList l;
    QString s;
    /*for (int i=0; i<model()->rowCount(); i++){
        max=1;
        for (int j=0; j<n; j++){
            s=model()->data(model()->index(i,j)).toString();
            l=s.split("\n");
            if (max<l.size()) max=l.size();
        }
        setRowHeight(i,max*ui->tableView->fontMetrics().height()+12);
    }*/
    for (int i=0; i<n; i++){
        s=model()->headerData(i,Qt::Horizontal).toString();
        max=horizontalHeader()->fontMetrics().width(s);
        for (int j=0; j<m; j++){
            s=model()->data(model()->index(j,i)).toString();
            l=s.split("\n");
            for (int k=0; k<l.size(); k++){
                if (max<fontMetrics().width(l.at(k)))
                    max=fontMetrics().width(l.at(k));
            }
        }
        setColumnWidth(i,max+12);
    }
}

void TableView::save(QString fnam)
{
    int rows,cols;
    int i,j;
    rows=this->model()->rowCount();
    cols=this->model()->columnCount();

    if (rows*cols>1){
        workbook wb;
        worksheet *sh = wb.sheet("sheet");
        cell_t *cref;

        QString hCubeell;

        //sh->merge(0,0,0,cols);
        sh->label(0,0,fnam.toStdWString());
        cref=sh->FindCellOrMakeBlank(0,0);
        cref->fontbold(BOLDNESS_DOUBLE);
        //cref->halign(HALIGN_CENTER);
        sh->rowheight(0,300);

        sh->rowheight(1,(this->horizontalHeader()->sizeHint().height())*30);

        sh->colwidth(0,(this->verticalHeader()->sizeHint().width())*38);

        sh->FindCellOrMakeBlank(1,0)->borderstyle(BORDER_LEFT, BORDER_HAIR);
        sh->FindCellOrMakeBlank(1,0)->borderstyle(BORDER_RIGHT, BORDER_HAIR);
        sh->FindCellOrMakeBlank(1,0)->borderstyle(BORDER_TOP, BORDER_HAIR);
        sh->FindCellOrMakeBlank(1,0)->borderstyle(BORDER_BOTTOM, BORDER_HAIR);

        for(i=0;i<cols;i++){
            hCubeell=this->model()->headerData(i,Qt::Horizontal).toString();
            hCubeell.replace(QChar('\n'),QChar('\n'));
            sh->label(1,i+1,hCubeell.toStdWString());
            cref=sh->FindCellOrMakeBlank(1,i+1);

            cref->borderstyle(BORDER_LEFT, BORDER_HAIR);
            cref->borderstyle(BORDER_RIGHT, BORDER_HAIR);
            cref->borderstyle(BORDER_TOP, BORDER_HAIR);
            cref->borderstyle(BORDER_BOTTOM, BORDER_HAIR);

            cref->fontbold(BOLDNESS_DOUBLE);
            cref->halign(HALIGN_JUSTIFY);
            sh->colwidth(i+1,(this->columnWidth(i)*38));
        }

        for(j=0;j<rows;j++){
            sh->label(j+2,0,this->model()->headerData(j,Qt::Vertical).toString().toStdWString());
            cref=sh->FindCellOrMakeBlank(j+2,0);
            cref->borderstyle(BORDER_LEFT, BORDER_HAIR);
            cref->borderstyle(BORDER_RIGHT, BORDER_HAIR);
            cref->borderstyle(BORDER_TOP, BORDER_HAIR);
            cref->borderstyle(BORDER_BOTTOM, BORDER_HAIR);
            cref->halign(HALIGN_LEFT);
        }

        for (i=0;i<rows;i++)
            for(j=0;j<cols;j++){
                int role=Qt::EditRole;
                QVariant value=this->model()->data(this->model()->index(i,j),role);
                if ((value.typeName()==QString("double"))||value.typeName()==QString("int")){
                    sh->number(i+2,j+1,value.toDouble());
                } else {
                    value=this->model()->data(this->model()->index(i,j),Qt::DisplayRole).toString();
                    sh->label(i+2,j+1,value.toString().toStdWString());
                }
                sh->FindCellOrMakeBlank(i+2,j+1)->borderstyle(BORDER_LEFT, BORDER_HAIR);
                sh->FindCellOrMakeBlank(i+2,j+1)->borderstyle(BORDER_RIGHT, BORDER_HAIR);
                sh->FindCellOrMakeBlank(i+2,j+1)->borderstyle(BORDER_TOP, BORDER_HAIR);
                sh->FindCellOrMakeBlank(i+2,j+1)->borderstyle(BORDER_BOTTOM, BORDER_HAIR);
            }

        QDir dir(QDir::homePath()+"/provreport");
        if (!dir.exists()) dir.mkdir(dir.path());
        QString filename = QFileDialog::getSaveFileName(this,tr("Сохранить документ"),
                                                        dir.path()+"/"+fnam+".xls",
                                                        tr("Documents (*.xls)") );
        if (!filename.isEmpty()){
            QFile file(filename);
            if (file.exists()) file.remove();
            std::string fil(filename.toLocal8Bit());
            wb.Dump(fil);
        }
    }
}
