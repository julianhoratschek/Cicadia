#ifndef PLOTDIALOG_H
#define PLOTDIALOG_H

#include "qcustomplot.h"

#include "../Algorithms/histogram.h"

#include <QDialog>

namespace Ui {
class PlotDialog;
}

class PlotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlotDialog(QWidget *parent = nullptr);
    explicit PlotDialog(QWidget *parent, const Histogram &h);
    ~PlotDialog();

public slots:
    void oncustomPlot_MousePress(QMouseEvent *event);
    void oncustomPlot_MouseWheel(QWheelEvent *event);

private:
    Ui::PlotDialog *ui;
};

#endif // PLOTDIALOG_H
