#ifndef PLOTDIALOG_H
#define PLOTDIALOG_H

#include "qcustomplot.h"

#include "../Algorithms/histogram.h"

#include <QDialog>
#include <QSvgGenerator>

namespace Ui {
class PlotDialog;
}


/**
 * @brief The PlotDialog class
 */
class PlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlotDialog(QWidget *parent = nullptr);
    explicit PlotDialog(QWidget *parent, const Histogram &h);
    explicit PlotDialog(QWidget *parent, const CCDoubleDataPtr &data);
    ~PlotDialog();



public slots:
    void oncustomPlot_MouseWheel(QWheelEvent *event);

private slots:
    void on_closePushButton_clicked();

    void on_savePushButton_clicked();

private:
    Ui::PlotDialog *ui;
};

#endif // PLOTDIALOG_H
