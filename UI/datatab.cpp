#include "datatab.h"
#include "ui_datatab.h"

const QString       DataTab::timeFormat = "yy-MM-dd HH:mm";

DataTab::DataTab(QWidget *parent, CCDataBase *_dataBase) :
    QWidget(parent),
    ui(new Ui::DataTab),
    dataBase(_dataBase),
    dataTableModel(nullptr),
    statisticsTableModel(nullptr),
    currentColumn(0)
{
    ui->setupUi(this);

    options.histogramClassCount = 10;
    options.cosinorTimePeriod = SecsInDay;
    options.cosinorRunsTestRuns = 5;

    ui->statisticsTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->statisticsTableView->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}


DataTab::~DataTab()
{
    delete dataTableModel;
    delete statisticsTableModel;

    delete ui;
}


void DataTab::setDataTableModel(DataTableModel *model)
{
    if(statisticsTableModel) {
        delete statisticsTableModel;
        statisticsTableModel = nullptr;
    }

    if(dataTableModel) {
        delete dataTableModel;
        dataTableModel = nullptr;
    }

    dataTableModel = model;
    statisticsTableModel = new StatisticsTableModel(this, dataTableModel);

    connect(dataTableModel, &DataTableModel::datasetRemoved, statisticsTableModel, &StatisticsTableModel::removeDataset);

    ui->dataTableView->setModel(dataTableModel);
    ui->statisticsTableView->setModel(statisticsTableModel);
}


void DataTab::addDataset(QSharedPointer<CCDataSet> dataset)
{
    if(dataset && dataTableModel) {
        dataTableModel->insertDataset(dataset);
        if(dataset->isType(static_cast<CCDataSet::DataType>(AlgorithmType::Cosinor))) {
            if(!dataset->getStatistics()) {
                CosinorData     *stats = new CosinorData();
                stats->load(dataBase->selectStatistics(dataset->getId()));
                Cosinor         c(stats, dataset, dataBase->selectDataset(dataset->getParentId()));
                c.recalc(0.05);
                dataset->setStatistics(stats);
            }
            statisticsTableModel->addItem(dataTableModel->columnCount() - 1, AlgorithmType::Cosinor);
        }
    }
}


QSharedPointer<CCDataSet> DataTab::columnSelectionToDataset(const QModelIndexList &column)
{
    CCDataSetPtr            ret(new CCDataSet(*dataTableModel->getDataset(column.first().column())));

    ret->setRange(column.first().data(KeyRole).toLongLong(),
                  column.last().data(KeyRole).toLongLong());
    ret->setSuffix("(" + QDateTime::fromSecsSinceEpoch(ret->from()).toString(timeFormat) + " to " +
                   QDateTime::fromSecsSinceEpoch(ret->to()).toString(timeFormat) + ")");

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
    QMenu           contextMenu, *dataMenu, *plotMenu, *methodsMenu;

    currentColumn = ui->dataTableView->columnAt(pos.x());

    dataMenu = contextMenu.addMenu("Data Manipulation");
        dataMenu->addAction(ui->actionCreate_Dataset);
        dataMenu->addAction(ui->actionSplit_Days);
        dataMenu->addSeparator();
        dataMenu->addAction(ui->actionChange_Color);
        dataMenu->addAction(ui->actionExport_Data);
        dataMenu->addSeparator();
        dataMenu->addAction(ui->actionRemove_Dataset);
    methodsMenu = contextMenu.addMenu("Calculate");
        methodsMenu->addAction(ui->actionSingle_Component_Cosinor);
    plotMenu = contextMenu.addMenu("Plots");
        plotMenu->addAction(ui->actionPlot_Data);
        plotMenu->addAction(ui->actionHistogram);
        plotMenu->addAction(ui->actionRankit);
        plotMenu->addAction(ui->actionPlot_Variances);
    contextMenu.addAction(ui->actionShow_Options);

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
    OptionsDialog           diag(this, &options);

    diag.exec();

    Cosinor::runsTestRuns = options.cosinorRunsTestRuns;
}

void DataTab::on_actionSplit_Days_triggered()
{
    auto            sorted = sortSelectedModelIndexesByColumns();

    for(auto col: sorted) {
        for(qint64 dtFrom = col.first().data(KeyRole).toLongLong(), dtTo = dtFrom + SecsInDay;
            dtTo < col.last().data(KeyRole).toLongLong(); dtTo += SecsInDay) {
                CCDataSetPtr        dataset(new CCDataSet(*dataTableModel->getDataset(col.first().column())));
                dataset->setRange(dtFrom, dtTo);
                dataset->setSuffix("(" + QDateTime::fromSecsSinceEpoch(dataset->from()).toString(timeFormat) + " to " +
                                   QDateTime::fromSecsSinceEpoch(dataset->to()).toString(timeFormat) + ")");
                dtFrom = dtTo;

               if(dataTableModel->insertDataset(dataBase->insertDataset(dataset, dataset->getDataId())))
                   emit datasetInserted(dataset);
        }
    }
}

void DataTab::on_actionHistogram_triggered()
{
    auto            sorted = sortSelectedModelIndexesByColumns();
    CCDataSetPtr    dataset = columnSelectionToDataset(sorted.first().values());

    Histogram       h(dataset, options.histogramClassCount);
    PlotDialog      pd(this, h);

    pd.exec();
}

void DataTab::on_actionRemove_Dataset_triggered()
{
    dataTableModel->removeDataset(currentColumn);
}

void DataTab::on_actionSingle_Component_Cosinor_triggered()
{
    auto            sorted = sortSelectedModelIndexesByColumns();

    for(auto cols: sorted) {
        CCDataSetPtr        dataset = columnSelectionToDataset(cols.values());
        Cosinor             c(dataset, options.cosinorTimePeriod);

        c.recalc(0.05);

        CCDataPtr           data = c.getData();
        CCDataSetPtr        insert = dataBase->insertData(data, dataset->getParentId(), c.getName(), dataset->getColor(), CCDataSet::ProcessedData | AlgorithmType::Cosinor);

        insert->setSuffix("Raw");

        insert->setStatistics(c.getPack());
        dataBase->insertStatistics(insert->getId(), insert->getStatistics()->save());

        if(dataTableModel->insertDataset(insert)) {
            statisticsTableModel->addItem(dataTableModel->columnCount()-1, AlgorithmType::Cosinor);
            emit datasetInserted(insert);
        }
    }
}

void DataTab::on_actionRankit_triggered()
{
    CCDataSetPtr    dataset = dataTableModel->getDataset(currentColumn),
                    parentset = dataBase->selectDataset(dataset->getParentId());

    if(!parentset->getData())
        parentset->setData(dataBase->selectData(parentset->getDataId()));

    if(!dataset->isType(static_cast<CCDataSet::DataType>(AlgorithmType::SingleComponentCosinor))) {
        QMessageBox::warning(this, "Error", "Column must be of Type 'Cosinor'");
        return;
    }

    Cosinor         c(static_cast<CosinorData*>(dataset->getStatistics()), dataset, parentset);
    PlotDialog      pd(this, c.rankitPlot());

    parentset.clear();

    pd.exec();
}

void DataTab::on_actionPlot_Variances_triggered()
{
    CCDataSetPtr        dataset = dataTableModel->getDataset(currentColumn),
                        parentset = dataBase->selectDataset(dataset->getParentId());

    if(!parentset->getData())
        parentset->setData(dataBase->selectData(parentset->getDataId()));

    if(!dataset->isType(static_cast<CCDataSet::DataType>(AlgorithmType::SingleComponentCosinor))) {
        QMessageBox::warning(this, "Error", "Column must be of Type 'Cosinor'");
        return;
    }

    Cosinor         c(static_cast<CosinorData*>(dataset->getStatistics()), dataset, parentset);
    PlotDialog      pd(this, c.variancePlot());

    parentset.clear();

    pd.exec();
}

void DataTab::on_actionPlot_Mesor_and_CI_triggered()
{
    emit plotCI(dataTableModel->getDataset(currentColumn));
}
