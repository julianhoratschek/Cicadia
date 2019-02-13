#include "datatab.h"
#include "ui_datatab.h"

DataTab::DataTab(QWidget *parent, CCDataBase *_dataBase) :
    QWidget(parent),
    ui(new Ui::DataTab),
    dataBase(_dataBase),
    dataTableModel(nullptr),
    statisticsTableModel(nullptr),
    currentColumn(0)
{
    ui->setupUi(this);
}


DataTab::~DataTab()
{
    delete dataTableModel;
    delete statisticsTableModel;

    delete ui;
}


void DataTab::setDataTableModel(DataTableModel *model)
{
    if(statisticsTableModel)
        delete statisticsTableModel;

    if(dataTableModel)
        delete dataTableModel;

    dataTableModel = model;
    statisticsTableModel = new StatisticsTableModel(this, dataTableModel);

    ui->dataTableView->setModel(dataTableModel);
    ui->statisticsTableView->setModel(statisticsTableModel);
}


void DataTab::addDataset(QSharedPointer<CCDataSet> dataset)
{
    if(dataset && dataTableModel)
        dataTableModel->insertDataset(dataset);
}


QSharedPointer<CCDataSet> DataTab::columnSelectionToDataset(const QModelIndexList &column)
{
    CCDataSetPtr            ret(new CCDataSet(*dataTableModel->getDataset(column.first().column())));

    ret->setRange(column.first().data(KeyRole).toLongLong(),
                  column.last().data(KeyRole).toLongLong());
    ret->setSuffix("Derived");

    return ret;
}


void DataTab::save(QDataStream &stream)
{
    stream << CCSerialization::CCSDataTab;

    dataTableModel->save(stream);
    statisticsTableModel->save(stream);
}

quint32 DataTab::load(QDataStream &stream)
{
    quint32     s;

    stream >> s;
    while(!stream.atEnd() && (s == CCSerialization::CCSDataTableModel || s == CCSerialization::CCSStatisticsTableModel)) {
        if( s == CCSerialization::CCSDataTableModel)
            dataTableModel->load(stream);
        else
            statisticsTableModel->load(stream);
        stream >> s;
    }

    return s;
}


void DataTab::on_actionChange_Color_triggered()
{
    auto            dataset = dataTableModel->getDataset(currentColumn);
    QColor          color = QColorDialog::getColor(dataset->getColor(), this, "Select Dataset Color");

    dataset->setColor(color);
    dataBase->updateDataset(*dataset);
}


void DataTab::on_dataTableView_customContextMenuRequested(const QPoint &pos)
{
    QMenu           contextMenu, *dataMenu, *plotMenu;

    currentColumn = ui->dataTableView->columnAt(pos.x());

    dataMenu = contextMenu.addMenu("Data Manipulation");
        dataMenu->addAction(ui->actionCreate_Dataset);
        dataMenu->addAction(ui->actionSplit_Days);
        dataMenu->addAction(ui->actionChange_Color);
        dataMenu->addAction(ui->actionExport_Data);
    plotMenu = contextMenu.addMenu("Plots");
        plotMenu->addAction(ui->actionPlot_Data);

    /*selectionMenu = contextMenu.addMenu("Selection");
        selectionMenu->addAction(ui->actionColumnsToDataset);
        selectionMenu->addAction(ui->actionPlotColumns);
        statsMenu = selectionMenu->addMenu("Statistics");
            statsMenu->addAction(ui->actionHanningSmooth);
            statsMenu->addAction(ui->actionCosinor);
            statsMenu->addAction(ui->actionCompare_Cosinor);
            statsMenu->addAction(ui->actionShow_Statistics);
            statsMenu->addAction(ui->actionShow_Histogramm);
            statsMenu->addAction(ui->actionPlot_Rankit);
            statsMenu->addAction(ui->actionVariance_Plot);
    columnMenu = contextMenu.addMenu("Column");
        columnMenu->addAction(ui->actionSetColumnColor);
        columnMenu->addAction(ui->actionExport_CSV);*/


    contextMenu.exec(ui->dataTableView->mapToGlobal(pos));
}


void DataTab::on_actionCreate_Dataset_triggered()
{
    auto            sorted = sortSelectedModelIndexesByColumns();

    for(auto col: sorted) {
        auto            dataset = columnSelectionToDataset(col.values());

        if(dataTableModel->insertDataset(dataBase->insertDataset(dataset, dataset->getDataId())))
            emit datasetInserted(dataset);
    }
}


/*
 *  Sorts the currently selected Indexes by Column
 *  Omits "null"-Values
 */
SortedModelIndexes DataTab::sortSelectedModelIndexesByColumns() const // OPTIMIZE?
{
    QModelIndexList                             list = ui->dataTableView->selectionModel()->selectedIndexes();
    SortedModelIndexes                          sorted;

    // Pre-sort selected Indexes by Column
    for(int i = 0; i < list.count(); i++) {
        if(list[i].data(UsedRole).toBool())
            sorted[list[i].column()][list[i].row()] = list[i];
    }

    return sorted;
}

void DataTab::on_actionPlot_Data_triggered()
{
    auto            sorted = sortSelectedModelIndexesByColumns();

    for(auto col: sorted)
        emit plotData(dataTableModel->getDataset(col.first().column()), col.values());
}

void DataTab::on_actionExport_Data_triggered()
{
    QString         fileName = QFileDialog::getSaveFileName(this, "Export to CSV", ".", "*.csv");
    QFile           fl(fileName);
    QTextStream     stream(&fl);
    auto            sorted = sortSelectedModelIndexesByColumns();

    fl.open(QIODevice::WriteOnly | QIODevice::Text);

    stream << "Time;Temp°C" << endl;
    for(auto col: sorted)
        for(auto row: col)
            stream << row.data(KeyRole).toLongLong() << ";" << row.data(Qt::DisplayRole).toDouble() << endl;

    fl.close();
}

void DataTab::on_actionShow_Options_triggered()
{
    OptionsDialog           diag;

    diag.exec();
}

void DataTab::on_actionSplit_Days_triggered()
{
    auto            sorted = sortSelectedModelIndexesByColumns();

    for(auto col: sorted) {
        for(quint64 dtFrom = col.first().data(KeyRole).toLongLong(), dtTo = dtFrom + SecsInDay;
            dtTo < col.last().data(KeyRole).toLongLong(); dtTo += SecsInDay) {
                CCDataSetPtr        dataset(new CCDataSet(*dataTableModel->getDataset(col.first().column())));
                dataset->setRange(dtFrom, dtTo);
                dtFrom = dtTo;

                dataTableModel->insertDataset(dataBase->insertDataset(dataset, dataset->getDataId()));
        }
    }
}

void DataTab::on_actionHistogram_triggered()
{
    auto            sorted = sortSelectedModelIndexesByColumns();

    Histogram       h(*columnSelectionToDataset(sorted.first().values()), 5);
    PlotDialog      pd(this, h);

    pd.exec();
}
