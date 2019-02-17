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

            for(int i=0;i<l.length();i+=2) {
                out << qSetFieldWidth(25) << left << "\n" + l[i] << left << l[i+1];
            }

            return s;
        }
    }
    else if(role == Qt::BackgroundRole)
        return dataTable->getDataset(items[index.row()].dataTableColumn)->getColor();

    return QVariant();
}


bool StatisticsTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(index.column() != AlphaColumn)
        return false;

    CCDataSetPtr    dataset = dataTable->getDataset(items[index.row()].dataTableColumn);
    Cosinor         c(static_cast<CosinorData*>(dataset->getStatistics()),
                      dataset, dataTable->getDatabase()->selectDataset(dataset->getParentId()));

    items[index.row()].alpha = value.toString().toDouble();
    c.recalc(items[index.row()].alpha);

    emit dataChanged(index, index.siblingAtColumn(DataColumn), {Qt::DisplayRole});

    return true;
}


bool StatisticsTableModel::addItem(int dataTableColumn, AlgorithmType type)
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
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    items.remove(row);
    endRemoveRows();

    return true;
}

Qt::ItemFlags StatisticsTableModel::flags(const QModelIndex &index) const
{
    if(index.column() == AlphaColumn)
        return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
    return QAbstractTableModel::flags(index);
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

void StatisticsTableModel::removeDataset(int column)
{
    int     r = -1;

    for(int i = 0;i < items.size(); i++) {
        if(items[i].dataTableColumn == column)
            r = i;
        else if(items[i].dataTableColumn > column)
            items[i].dataTableColumn--;
    }

    if(r >= 0)
        removeRows(r, 1);
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

    type = static_cast<AlgorithmType>(tp);
}
