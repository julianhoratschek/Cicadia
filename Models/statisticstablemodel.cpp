#include "statisticstablemodel.h"

StatisticsTableModel::StatisticsTableModel(QObject *parent, DataTableModel *_dataTable)
    : QAbstractTableModel(parent), dataTable(_dataTable)
{
}


/**
 * @brief StatisticsTableModel::headerData
 * @param section
 * @param orientation
 * @param role
 * @return
 */
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


/**
 * @brief StatisticsTableModel::rowCount
 * @param parent
 * @return
 */
int StatisticsTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return items.count();
}


/**
 * @brief StatisticsTableModel::columnCount
 * @param parent
 * @return
 */
int StatisticsTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return StatisticsTableModel_ColumnCount;
}


/**
 * @brief StatisticsTableModel::data
 * @param index
 * @param role
 * @return
 */
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
            QStringList     l = dataTable->getDataset(items[index.row()].dataTableColumn)->getStatistics()->toStringList();

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


/**
 * @brief StatisticsTableModel::setData
 * @param index
 * @param value
 * @param role
 * @return
 */
bool StatisticsTableModel::setData(const QModelIndex &index, const QVariant &value, int)
{
    if(index.column() != AlphaColumn)
        return false;

    CCDataSetPtr    dataset = dataTable->getDataset(items[index.row()].dataTableColumn);
    Cosinor         c(dynamic_cast<CosinorData*>(dataset->getStatistics()),
                      dataset, dataTable->getDatabase()->selectDataset(dataset->getParentId()));

    items[index.row()].alpha = value.toString().toDouble();
    c.recalc(items[index.row()].alpha);

    emit dataChanged(index, index.siblingAtColumn(DataColumn), {Qt::DisplayRole});

    return true;
}


/**
 * @brief StatisticsTableModel::addItem
 * @param dataTableColumn
 * @param type
 * @return
 */
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


/**
 * @brief StatisticsTableModel::removeRows
 * @param row
 * @param count
 * @return
 */
bool StatisticsTableModel::removeRows(int row, int count, const QModelIndex &)
{
    beginRemoveRows(QModelIndex(), row, row + count - 1);
    items.remove(row);
    endRemoveRows();

    return true;
}


/**
 * @brief StatisticsTableModel::flags
 * @param index
 * @return
 */
Qt::ItemFlags StatisticsTableModel::flags(const QModelIndex &index) const
{
    if(index.column() == AlphaColumn)
        return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
    return QAbstractTableModel::flags(index);
}


/**
 * @brief StatisticsTableModel::save
 * @param stream
 */
void StatisticsTableModel::save(QDataStream &stream)
{
    stream << CCSerialization::CCSStatisticsTableModel << items.count();

    for(auto const &it: items) {
        it.save(stream);
        stream << dataTable->getDataset(it.dataTableColumn)->getStatistics()->save();
    }
}


/**
 * @brief StatisticsTableModel::load
 * @param stream
 * @return
 */
qint32 StatisticsTableModel::load(QDataStream &stream)
{
    qint32             sz, tp;

    items.clear();

    stream >> sz;

    for(qint32 i = 0; i < sz; i++) {
        StatisticsTableItem it;
        QString             ld;
        CosinorData         dt;

        stream >> tp;
        if(tp != CCSerialization::CCSStatisticsTableItem)
            return tp;
        it.load(stream);
        items << it;

        stream >> ld;
        dt.load(ld);
        dataTable->getDataset(it.dataTableColumn)->setStatistics(new CosinorData(dt));
    }

    return 0;
}


/**
 * @brief StatisticsTableModel::removeDataset
 * @param column
 */
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


/**
 * @brief StatisticsTableItem::save
 * @param stream
 */
void StatisticsTableItem::save(QDataStream &stream) const
{
    stream << CCSerialization::CCSStatisticsTableItem;
    stream << static_cast<qint32>(dataTableColumn) << alpha << static_cast<qint32>(type);
}


/**
 * @brief StatisticsTableItem::load
 * @param stream
 */
void StatisticsTableItem::load(QDataStream &stream)
{
    qint32         tp;

    stream >> tp;
    dataTableColumn = tp;
    stream >> alpha;
    stream >> tp;
    type = static_cast<AlgorithmType>(tp);
}
