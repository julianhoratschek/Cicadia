#ifndef DATATAB_H
#define DATATAB_H

#include "Models/statisticstablemodel.h"

#include "plotdialog.h"
#include "optionsdialog.h"

#include "../Algorithms/histogram.h"
#include "../Algorithms/cosinor.h"

#include <QWidget>
#include <QMenu>

#include <QFileDialog>
#include <QColorDialog>

namespace Ui {
class DataTab;
}

typedef QMap<int, QMap<int, QModelIndex>>       SortedModelIndexes;

class DataTab : public QWidget
{
    Q_OBJECT

public:
    explicit DataTab(QWidget *parent = nullptr, CCDataBase *_dataBase = nullptr);
    ~DataTab();

    void setDataTableModel(DataTableModel *model);
    void addDataset(CCDataSetPtr dataset);
    CCDataSetPtr        columnSelectionToDataset(const QModelIndexList &column);

    void save(QDataStream &stream);
    quint32 load(QDataStream &stream);


signals:
    void plotData(CCDataSetPtr dataset, const QModelIndexList &col);
    void plotCI(CCDataSetPtr dataset);
    void datasetInserted(CCDataSetPtr dataset);


private slots:
    void on_actionChange_Color_triggered();

    void on_dataTableView_customContextMenuRequested(const QPoint &pos);

    void on_actionCreate_Dataset_triggered();

    void on_actionPlot_Data_triggered();

    void on_actionExport_Data_triggered();

    void on_actionShow_Options_triggered();

    void on_actionSplit_Days_triggered();

    void on_actionHistogram_triggered();

    void on_actionRemove_Dataset_triggered();

    void on_actionSingle_Component_Cosinor_triggered();

    void on_actionRankit_triggered();

    void on_actionPlot_Variances_triggered();

    void on_actionPlot_Mesor_and_CI_triggered();

private:
    static const QString        timeFormat;

    Ui::DataTab                 *ui;

    CCDataBase                  *dataBase;
    DataTableModel              *dataTableModel;
    StatisticsTableModel        *statisticsTableModel;

    int                         currentColumn;
    TabOptions                  options;

    SortedModelIndexes sortSelectedModelIndexesByColumns() const;
};

#endif // DATATAB_H
