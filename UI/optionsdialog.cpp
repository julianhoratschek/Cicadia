#include "optionsdialog.h"
#include "ui_optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent, TabOptions *_options) :
    QDialog(parent),
    ui(new Ui::OptionsDialog),
    options(_options)
{
    ui->setupUi(this);

    ui->optionsTableWidget->item(HistogramClassRow, 0)->setText(QString::number(options->histogramClassCount));
    ui->optionsTableWidget->item(CosinorTimePeriodRow, 0)->setText(QString::number(options->cosinorTimePeriod));
    ui->optionsTableWidget->item(CosinorRunsTestRunsRow, 0)->setText(QString::number(options->cosinorRunsTestRuns));
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::on_optionsTableWidget_itemChanged(QTableWidgetItem *item)
{
    switch(item->row()) {
    case HistogramClassRow:
        options->histogramClassCount = item->text().toInt();
        break;
    case CosinorTimePeriodRow:
        options->cosinorTimePeriod = item->text().toInt();
        break;
    case CosinorRunsTestRunsRow:
        options->cosinorRunsTestRuns = item->text().toInt();
    }
}
