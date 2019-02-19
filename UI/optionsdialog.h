#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>
#include <QTableWidgetItem>

namespace Ui {
class OptionsDialog;
}


/**
 * @brief The TabOptions struct
 */
struct TabOptions
{
    int         histogramClassCount,
                cosinorTimePeriod,
                cosinorRunsTestRuns;

    void save(QDataStream &stream) {
        stream << static_cast<qint32>(histogramClassCount) << static_cast<qint32>(cosinorTimePeriod) << static_cast<qint32>(cosinorRunsTestRuns);
    }

    void load(QDataStream &stream) {
        qint32      s;
        stream >> s;
        histogramClassCount = s;
        stream >> s;
        cosinorTimePeriod = s;
        stream >> s;
        cosinorRunsTestRuns = s;
    }
};


/**
 * @brief The OptionsDialog class
 */
class OptionsDialog : public QDialog
{
    Q_OBJECT

public:

    // Public Declarations

    enum OptionRows {
        HistogramClassRow = 0,
        CosinorTimePeriodRow = 1,
        CosinorRunsTestRunsRow = 2
    };


    // Public Methods

    explicit OptionsDialog(QWidget *parent = nullptr, TabOptions *_options = nullptr);
    ~OptionsDialog();

private slots:
    void                on_optionsTableWidget_itemChanged(QTableWidgetItem *item);

private:

    // Private Members

    Ui::OptionsDialog   *ui;
    TabOptions          *options;
};

#endif // OPTIONSDIALOG_H
