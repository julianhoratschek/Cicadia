#ifndef STATISTICSTABLEMODEL_H
#define STATISTICSTABLEMODEL_H

#include "datatablemodel.h"

#include "../Algorithms/algorithmbase.h"


struct StatisticsTableItem {
    int                             dataTableColumn;
    QMap<QString, double>           display;
    double                          alpha;
    AlgorithmBase::AlgorithmType    type;

    StatisticsTableItem()
        : dataTableColumn(0), alpha(0.05), type(AlgorithmBase::SingleComponentCosinor) {}
    StatisticsTableItem(const StatisticsTableItem &other)
        : dataTableColumn(other.dataTableColumn), display(other.display), alpha(other.alpha), type(other.type) {}
};


class StatisticsTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:

#define StatisticsTableModel_ColumnCount        3
    enum ColumnTypes {
        AlphaColumn = 0,
        NameColumn,
        DataColumn
    };

    explicit StatisticsTableModel(QObject *parent = nullptr, DataTableModel *_dataTable = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool addItem(int dataTableColumn, AlgorithmBase::AlgorithmType type);

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    DataTableModel                      *dataTable;
    QVector<StatisticsTableItem>        items;
};


#endif // STATISTICSTABLEMODEL_H
