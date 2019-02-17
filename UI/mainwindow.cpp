#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dataBase = new CCDataBase();

    subjectsTreeModel = new SubjectsTreeModel(this, dataBase);
    ui->subjectsTreeView->setModel(subjectsTreeModel);
    ui->subjectsTreeView->setColumnHidden(SubjectsTreeModel::IDColumn, true);
    ui->subjectsTreeView->setColumnHidden(SubjectsTreeModel::TypeColumn, true);

    // Setup default Tab
    ui->tabWidget->tabBar()->tabButton(0, QTabBar::RightSide)->hide();

    //ui->tabWidget->tabBar()->moveTab(1, 0);
    insertDataTab(0);
    ui->tabWidget->setCurrentIndex(0);

    connect(ui->tabWidget->tabBar(), &QTabBar::tabMoved, this, &MainWindow::ontabBar_tabMoved);

    // Setup customPlot
    QSharedPointer<QCPAxisTickerTime>       dtLabel(new QCPAxisTickerTime);
    QSharedPointer<QCPAxisTicker>           tLabel(new QCPAxisTicker);

    ui->customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));

    dtLabel->setTimeFormat("Day %d\n%h:%m");
    dtLabel->setTickCount(24);
    dtLabel->setTickStepStrategy(QCPAxisTicker::tssReadability);
    ui->customPlot->xAxis->setTicker(dtLabel);

    tLabel->setTickCount(15);
    tLabel->setTickStepStrategy(QCPAxisTicker::tssReadability);
    ui->customPlot->yAxis->setTicker(tLabel);

    ui->customPlot->xAxis->setLabel("Time");
    ui->customPlot->yAxis->setLabel("Temperature °C");      //TODO: Different labels?

    ui->customPlot->setInteraction(QCP::iSelectPlottables, true);
    ui->customPlot->setInteraction(QCP::iMultiSelect, true);
    ui->customPlot->setSelectionRectMode(QCP::srmSelect);

    ui->customPlot->setInteraction(QCP::iRangeDrag, true);
    ui->customPlot->setInteraction(QCP::iRangeZoom, true);
    ui->customPlot->axisRect()->setRangeZoomAxes(ui->customPlot->xAxis, nullptr);

    ui->customPlot->legend->setVisible(true);

    connect(ui->customPlot, &QCustomPlot::mouseWheel, this, &MainWindow::oncustomPlot_MouseWheel);
    connect(ui->customPlot, &QCustomPlot::mousePress, this, &MainWindow::oncustomPlot_MousePress);
}


MainWindow::~MainWindow()
{
    delete subjectsTreeModel;
    delete dataBase;
    delete ui;
}

/*
 *      Adds new DataTab to TabView
 */
void MainWindow::insertDataTab(int index) {
    auto                *tab = new DataTab(this, dataBase);
    auto                *model = new DataTableModel(this, dataBase);

    tab->setDataTableModel(model);
    ui->tabWidget->insertTab(index, tab, "Dataset");

    connect(tab, &DataTab::plotData, this, &MainWindow::plotData);
    connect(tab, &DataTab::plotCI, this, &MainWindow::plotCI);
    connect(tab, &DataTab::datasetInserted, subjectsTreeModel, &SubjectsTreeModel::insertDataset);
    connect(subjectsTreeModel, &SubjectsTreeModel::removedDataset, model, &DataTableModel::removeDataset);


    /*connect(tab, &DataTab::dataSetInserted, this, &MainWindow::ondataTab_dataSetInserted);


    connect(tab, &QCDDataTab::plotMesor, this, &MainWindow::ondataTab_plotMesor);
    connect(tab, &QCDDataTab::plotHistogramm, this, &MainWindow::ondataTab_plotHistogramm);
    connect(tab, &QCDDataTab::plotRankit, this, &MainWindow::ondataTab_plotRankit);
    connect(tab, &QCDDataTab::plotVariance, this, &MainWindow::ondataTab_plotVariance);

    connect(tab, &QCDDataTab::columnColorChanged, this, &MainWindow::ondataTab_columnColorChanged);*/
}

void MainWindow::plotData(CCDataSetPtr dataset, const QModelIndexList &list)
{
    QPen                    pen;
    QVector<double>         keys, values;

    QDateTime               dt = QDateTime::fromSecsSinceEpoch(dataset->from());
    dt.setTime(QTime::fromMSecsSinceStartOfDay(0));

    ui->customPlot->addGraph();
    ui->customPlot->graph()->setName(dataset->getName());

    pen.setColor(dataset->getColor());
    ui->customPlot->graph()->setPen(pen);

    ui->customPlot->graph()->setSelectable(QCP::stSingleData);

    for(auto it: list) {
        keys << it.data(KeyRole).toLongLong() - dt.toSecsSinceEpoch();
        values << it.data().toDouble();
    }

    ui->customPlot->graph()->setData(keys, values, false);

    graphRelations.insert(ui->customPlot->graph(), dataset);

    ui->customPlot->rescaleAxes();

    ui->customPlot->replot();
}

void MainWindow::plotCI(const QSharedPointer<CCDataSet> &dataset)
{
    CCDataSetPtr        parentset = dataBase->selectDataset(dataset->getParentId());

    if(!dataset->isType(static_cast<CCDataSet::DataType>(AlgorithmType::SingleComponentCosinor))) {
        QMessageBox::warning(this, "Error", "Column must be of Type 'Cosinor'");
        return;
    }

    Cosinor             c(static_cast<CosinorData*>(dataset->getStatistics()), dataset, parentset);
    CCDoubleDataPtr     upper, lower;

    c.MesorCI(upper, lower);

    QCPGraph                    *ugraph = ui->customPlot->addGraph(),
                                *lgraph = ui->customPlot->addGraph();
    QVector<double>             uvalues, lvalues, keys;
    QDateTime                   dt = QDateTime::fromSecsSinceEpoch(dataset->from());
    dt.setTime(QTime::fromMSecsSinceStartOfDay(0));

    QPen pen;

    pen.setStyle(Qt::DotLine);
    pen.setColor(dataset->getColor());
    ugraph->setName("MesorCI " + dataset->getName());
    ugraph->setPen(pen);
    ugraph->setBrush(QBrush(QColor(dataset->getColor().lighter(300))));

    //ui->customPlot->legend->removeItem(ui->customPlot->legend->itemCount()-1);
    lgraph->setPen(pen);
    ugraph->setChannelFillGraph(lgraph);

    for(auto it = dataset->begin(); it != dataset->end(); it++) {
        keys << it.key() - dt.toSecsSinceEpoch();
        uvalues << upper->at(it.key());
        lvalues << lower->at(it.key());
    }

    ugraph->setData(keys, uvalues);
    lgraph->setData(keys, lvalues);

    ui->customPlot->rescaleAxes();

    ui->customPlot->replot();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_Delete:
        for(auto graph: ui->customPlot->selectedGraphs()) {
            for(auto range: graph->selection().dataRanges()) {
                double      begin = graph->data()->at(range.begin())->key,
                            end = graph->data()->at(range.end())->key;
                QDateTime   dt = QDateTime::fromSecsSinceEpoch(graphRelations[graph]->from());
                dt.setTime(QTime::fromMSecsSinceStartOfDay(0));

                dataBase->hideData(graphRelations[graph],
                                   dt.toSecsSinceEpoch() + begin,
                                   dt.toSecsSinceEpoch() + end);
                graph->data()->remove(begin, end);
            }
        }
        ui->customPlot->replot();
        break;
    default:
        QMainWindow::keyPressEvent(event);
    }
}


/*
 *  Prohibit moving the "+"-Button Tab or add new Datasets
 */
void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    if(index == ui->tabWidget->count()-1) {
        insertDataTab(index);
        ui->tabWidget->setCurrentIndex(index);

        ui->tabWidget->setMovable(false);
    }
    else
        ui->tabWidget->setMovable(true);
}

/*
 *  Prohibit moving other Tabs past the "+"-Button Tab
 */
void MainWindow::ontabBar_tabMoved(int, int)
{
    if(ui->tabWidget->indexOf(ui->addTabWidget) != ui->tabWidget->count() - 1)
        ui->tabWidget->tabBar()->moveTab(ui->tabWidget->indexOf(ui->addTabWidget), ui->tabWidget->count() - 1);
}


/*
 *  Closing the "+"-Tab is not allowed, if the last Tab except for it is closed,
 *  another DataTab will be inserted
 */
void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if(index < ui->tabWidget->count() - 1) {
        QWidget     *w = ui->tabWidget->widget(index);

        ui->tabWidget->removeTab(index);
        delete w;

        if(ui->tabWidget->count() < 2) {
            insertDataTab(0);
            ui->tabWidget->setCurrentIndex(0);
        }
        else {
            ui->tabWidget->setCurrentIndex(index-1);
        }
    }
}


void MainWindow::on_subjectsTreeView_doubleClicked(const QModelIndex &index)
{
    auto            ptr = static_cast<SubjectsTreeItem*>(index.internalPointer());

    qobject_cast<DataTab*>(ui->tabWidget->currentWidget())->addDataset(ptr->dataset);
}

/*
 *  On holding [SHIFT] one can zoom on Y-Axis, otherwise on X-Axis
 */
void MainWindow::oncustomPlot_MouseWheel(QWheelEvent *event)
{
    if((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier )
        ui->customPlot->axisRect()->setRangeZoomAxes(nullptr, ui->customPlot->yAxis);
    else
        ui->customPlot->axisRect()->setRangeZoomAxes(ui->customPlot->xAxis, nullptr);
}


/*
 *  Holding [SHIFT] enables Selection-Mode
 */
void MainWindow::oncustomPlot_MousePress(QMouseEvent *event)
{
    if((event->buttons() & Qt::LeftButton) == Qt::LeftButton) {
        if((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier)
            ui->customPlot->setSelectionRectMode(QCP::srmSelect);
        else
            ui->customPlot->setSelectionRectMode(QCP::srmNone);
    }
}


void MainWindow::on_actionImport_triggered()
{
    QStringList         fileNames = QFileDialog::getOpenFileNames(this, "Import Files", ".", "*.csv");

    for(auto s: fileNames) {
        auto        dataset = dataBase->importFromFile(s);
        if(!dataset)
            QMessageBox::warning(this, "Error", "Could not load File " + s);
        else
            subjectsTreeModel->insertDataset(dataset);
    }
}


void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionExport_Graph_triggered()
{
    QSvgGenerator       gen;
    QCPPainter          painter;
    QString             fileName = QFileDialog::getSaveFileName(this, "Save Plot to", ".", "*.svg");

    gen.setFileName(fileName);

    painter.begin(&gen);
    ui->customPlot->toPainter(&painter);
    painter.end();

    QMessageBox::information(this, "Success", "Plot was saved");
}

void MainWindow::on_actionClear_Graph_triggered()
{
    graphRelations.clear();
    ui->customPlot->clearGraphs();
    ui->customPlot->replot();
}

void MainWindow::on_actionSave_triggered()
{
    QString     fileName = QFileDialog::getSaveFileName(this, "Save to", ".", "*.db");
    quint32     s = CCSerialization::CCSMagicNumber, v = CCSerialization::CCSVersion;

    dataBase->save(fileName);

    QFile       fl(fileName + ".dt");
    fl.open(QIODevice::WriteOnly);
    QDataStream stream(&fl);

    stream << s << v << (quint32)ui->tabWidget->count();
    for(int i = 0; i < ui->tabWidget->count(); i++)
        qobject_cast<DataTab*>(ui->tabWidget->widget(i))->save(stream);

    fl.close();
}

void MainWindow::on_actionOpen_triggered()
{
    QString     fileName = QFileDialog::getOpenFileName(this, "Open File", ".", "*.db");
    quint32     s, v;

    dataBase->load(fileName);

    QFile       fl(fileName + ".dt");
    fl.open(QIODevice::ReadOnly);
    QDataStream stream(&fl);

    stream >> s;
    stream >> v;

    if(s != CCSerialization::CCSMagicNumber || v > CCSerialization::CCSVersion) {
        QMessageBox::warning(this, "Error", "File Type or Version wrong");
        return;
    }

    stream >> s;

    for(quint64 i = 0; i < s; i++) {
        insertDataTab(ui->tabWidget->count() - 1);
        stream >> v;
        qobject_cast<DataTab*>(ui->tabWidget->widget(ui->tabWidget->count() - 1))->load(stream);
    }

    fl.close();
}

void MainWindow::on_subjectsTreeView_customContextMenuRequested(const QPoint &pos)
{
    QMenu           contextMenu;

    currentDataset = static_cast<SubjectsTreeItem*>(ui->subjectsTreeView->indexAt(pos).internalPointer())->dataset;

    if(!currentDataset)
        return;

    contextMenu.addAction(ui->actionDelete_Dataset);

    contextMenu.exec(ui->subjectsTreeView->mapToGlobal(pos));
}

void MainWindow::on_actionDelete_Dataset_triggered()
{
    if(QMessageBox::question(this, "Delete Dataset", "Delete " + currentDataset->getName() + " and all its Children?") == QMessageBox::Yes)
        subjectsTreeModel->deleteDataset(currentDataset);
}
