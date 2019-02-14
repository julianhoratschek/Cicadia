#include "statisticstablemodel.h"

StatisticsTableModel::StatisticsTableModel(QObject *parent, DataTableModel *_dataTable)
    : QAbstractTableModel(parent), dataTable(_dataTable)
{
}

QVariant StatisticsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch(section) {
        case NameColumn:
            return "Name";
        case AlphaColumn:
            return "Alpha";
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
        case NameColumn:
            return dataTable->headerData(items[index.row()].dataTableColumn, Qt::Horizontal);

        case AlphaColumn:
            return items[index.row()].alpha;

        case DataColumn:
            QString         s = "";
            QTextStream     out(&s);
            QStringList     l = dataTable->getDataset(items[index.row()].dataTableColumn)->getStatistics()->toString();

            out.setFieldWidth(25);

            for(int i=0;i<l.length();i+=2)
                out << l[i] << l[i+1] << endl;

            return s;
        }
    }
    else if(role == Qt::BackgroundRole)
        return dataTable->data(createIndex(0, items[index.row()].dataTableColumn), role);

    return QVariant();
}


bool StatisticsTableModel::addItem(int dataTableColumn, AlgorithmType::Name type)
{
    beginInsertRows(QModelIndex(), items.count(), items.count());
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

    return true;
}

void StatisticsTableModel::save(QDataStream &stream)
{
    stream << CCSerialization::CCSStatisticsTableModel;
    stream << (quint32)items.count();

    for(auto it: items)
        it.save(stream);
}

quint32 StatisticsTableModel::load(QDataStream &stream)
{
    quint32             sz, tp;

    items.clear();

    stream >> sz;

    for(quint32 i = 0; i < sz; i++) {
        StatisticsTableItem it;

        stream >> tp;
        if(tp != CCSerialization::CCSStatisticsTableItem)
            return tp;
        it.load(stream);
        items << it;
    }

    return 0;
}



void StatisticsTableItem::save(QDataStream &stream) const
{
    stream << CCSerialization::CCSStatisticsTableItem;
    stream << dataTableColumn << alpha << (quint32)type;
}

void StatisticsTableItem::load(QDataStream &stream)
{
    quint32         tp;

    stream >> dataTableColumn;
    stream >> alpha;
    stream >> tp;

    type = (AlgorithmType::Name)tp;
}
