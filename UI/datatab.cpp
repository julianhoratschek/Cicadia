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
    if(dataTableModel)
        dataTableModel->insertDataset(dataset);
}


QSharedPointer<CCDataSet> DataTab::columnSelectionToDataset(const QModelIndexList &column)
{
    CCDataSetPtr            ret(new CCDataSet(*dataTableModel->getDataset(column.first().column())));

    ret->setRange(column.first().data(KeyRole).toLongLong(),
                  column.last().data(KeyRole).toLongLong());

    return ret;
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
        dataMenu->addAction(ui->actionChange_Color);
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

        dataTableModel->insertDataset(dataBase->insertDataset(dataset, dataset->getDataId()););
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
        if(list[i].data() != "null")
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
