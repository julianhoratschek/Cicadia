#ifndef STATISTICSTABLEMODEL_H
#define STATISTICSTABLEMODEL_H

#include "datatablemodel.h"


struct StatisticsTableItem {
    int                                         dataTableColumn;
    double                                      alpha;
    AlgorithmType::Name                         type;

    StatisticsTableItem()
        : dataTableColumn(0), alpha(0.05), type(AlgorithmType::SingleComponentCosinor) {}
    StatisticsTableItem(const StatisticsTableItem &other)
        : dataTableColumn(other.dataTableColumn), alpha(other.alpha), type(other.type) {}

    void save(QDataStream &stream) const;
    void load(QDataStream &stream);
};


class StatisticsTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:

#define StatisticsTableModel_ColumnCount        3
    enum ColumnTypes {
        NameColumn = 0,
        AlphaColumn,
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
    bool addItem(int dataTableColumn, AlgorithmType::Name type);

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void save(QDataStream &stream);
    quint32 load(QDataStream &stream);

private:
    DataTableModel                      *dataTable;
    QVector<StatisticsTableItem>        items;
};


#endif // STATISTICSTABLEMODEL_H
