#ifndef DATATAB_H
#define DATATAB_H

#include "Modles/statisticstablemodel.h"

#include <QWidget>
#include <QMenu>

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


signals:
    void plotData(CCDataSetPtr dataset, const QModelIndexList &col);


private slots:
    void on_actionChange_Color_triggered();

    void on_dataTableView_customContextMenuRequested(const QPoint &pos);

    void on_actionCreate_Dataset_triggered();

    void on_actionPlot_Data_triggered();

private:
    Ui::DataTab                 *ui;

    CCDataBase                  *dataBase;
    DataTableModel              *dataTableModel;
    StatisticsTableModel        *statisticsTableModel;

    int                         currentColumn;

    SortedModelIndexes sortSelectedModelIndexesByColumns() const;
};

#endif // DATATAB_H
