#include "statisticstablemodel.h"

StatisticsTableModel::StatisticsTableModel(QObject *parent = nullptr, DataTableModel *_dataTable = nullptr)
    : QAbstractTableModel(parent), dataTable(_dataTable)
{
}

QVariant StatisticsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal) {
        switch(section) {
        case AlphaColumn:
            return "Alpha";
        case NameColumn:
            return "Name";
        case DataColumn:
            return "Data";
        }
    }

    return QVariant();
}


int StatisticsTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return items.count();
}


int StatisticsTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return StatisticsTableModel_ColumnCount;
}


QVariant StatisticsTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role == Qt::DisplayRole) {
        switch(index.column()) {
        case AlphaColumn:
            return items[index.row()].alpha;

        case NameColumn:
            return dataTable->headerData(items[index.row()].dataTableColumn, Qt::Horizontal);

        case DataColumn:
            QString         s = "";
            QTextStream     out(&s);

            out.setFieldWidth(25);

            for(auto it = items[index.row()].display.begin(); it != items[index.row()].display.end(); it++)
                out << it.key() << it.value();

            return s;
        }
    }
    else if(role == Qt::BackgroundRole)
        return dataTable->data(createIndex(0, items[index.row()].dataTableColumn), role);

    return QVariant();
}


bool StatisticsTableModel::addItem(int dataTableColumn, AlgorithmBase::AlgorithmType type)
{
    beginInsertRows(createIndex(0, 0), items.count(), items.count());
    StatisticsTableItem     item;

    item.dataTableColumn = dataTableColumn;
    item.type = type;

    items << item;

    endInsertRows();

    return true;
}


bool StatisticsTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    items.remove(row);
    endRemoveRows();
}


