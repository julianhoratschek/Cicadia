#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QTableWidgetItem>

namespace Ui {
class OptionsDialog;
}

struct TabOptions
{
    int         histogramClassCount, cosinorTimePeriod;
};

class OptionsDialog : public QDialog
{
    Q_OBJECT

public:
    enum OptionRows {
        HistogramClassRow = 0,
        CosinorTimePeriodRow = 1,
    };

    explicit OptionsDialog(QWidget *parent = nullptr, TabOptions *_options = nullptr);
    ~OptionsDialog();

private slots:
    void on_optionsTableWidget_itemChanged(QTableWidgetItem *item);

private:
    Ui::OptionsDialog   *ui;
    TabOptions          *options;
};

#endif // OPTIONSDIALOG_H
