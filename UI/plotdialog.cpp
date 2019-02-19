#include "plotdialog.h"
#include "ui_plotdialog.h"

PlotDialog::PlotDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PlotDialog)
{
    ui->setupUi(this);

    QSharedPointer<QCPAxisTicker>           XLabel(new QCPAxisTicker);
    QSharedPointer<QCPAxisTicker>           YLabel(new QCPAxisTicker);

    ui->customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom));

    XLabel->setTickCount(24);
    XLabel->setTickStepStrategy(QCPAxisTicker::tssReadability);
    ui->customPlot->xAxis->setTicker(XLabel);

    YLabel->setTickCount(15);
    YLabel->setTickStepStrategy(QCPAxisTicker::tssReadability);
    ui->customPlot->yAxis->setTicker(YLabel);

    ui->customPlot->xAxis->setLabel("X");
    ui->customPlot->yAxis->setLabel("Y");      //TODO: Different labels?

    /*ui->customPlot->setInteraction(QCP::iSelectPlottables, true);
    ui->customPlot->setInteraction(QCP::iMultiSelect, true);
    ui->customPlot->setSelectionRectMode(QCP::srmSelect);*/

    ui->customPlot->setInteraction(QCP::iRangeDrag, true);
    ui->customPlot->setInteraction(QCP::iRangeZoom, true);
    ui->customPlot->axisRect()->setRangeZoomAxes(ui->customPlot->xAxis, nullptr);

    ui->customPlot->legend->setVisible(false);

    connect(ui->customPlot, &QCustomPlot::mouseWheel, this, &PlotDialog::oncustomPlot_MouseWheel);
    //connect(ui->customPlot, &QCustomPlot::mousePress, this, &PlotDialog::oncustomPlot_MousePress);
}


/**
 * @brief PlotDialog::PlotDialog
 * @param parent
 * @param h
 */
PlotDialog::PlotDialog(QWidget *parent, const Histogram &h)
    : PlotDialog(parent)
{
    CCDoubleDataPtr         data = h.getData();
    QCPBars                 *myBars = new QCPBars(ui->customPlot->xAxis, ui->customPlot->yAxis);
    QVector<double>         k = data->keys(),
                            d = data->values();

    myBars->setData(k, d);
    if(h.stepWidth() != 0.0)
        myBars->setWidth(h.stepWidth());
    ui->customPlot->rescaleAxes();
}


/**
 * @brief PlotDialog::PlotDialog
 * @param parent
 * @param data
 */
PlotDialog::PlotDialog(QWidget *parent, const CCDoubleDataPtr &data)
    : PlotDialog(parent)
{
    QVector<double>         k = data->keys(),
                            d = data->values();
    ui->customPlot->addGraph();
    ui->customPlot->graph()->setLineStyle(QCPGraph::lsNone);
    ui->customPlot->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross));
    ui->customPlot->graph()->setData(k, d);
    ui->customPlot->rescaleAxes();
    ui->customPlot->replot();
}


/**
 * @brief PlotDialog::~PlotDialog
 */
PlotDialog::~PlotDialog()
{
    delete ui;
}


/**
 * @brief PlotDialog::oncustomPlot_MouseWheel On holding [SHIFT] one can zoom on Y-Axis, otherwise on X-Axis
 * @param event
 */
void PlotDialog::oncustomPlot_MouseWheel(QWheelEvent *event)
{
    if((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier )
        ui->customPlot->axisRect()->setRangeZoomAxes(nullptr, ui->customPlot->yAxis);
    else
        ui->customPlot->axisRect()->setRangeZoomAxes(ui->customPlot->xAxis, nullptr);
}


/**
 * @brief PlotDialog::on_closePushButton_clicked
 */
void PlotDialog::on_closePushButton_clicked()
{
    close();
}


/**
 * @brief PlotDialog::on_savePushButton_clicked
 */
void PlotDialog::on_savePushButton_clicked()
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
