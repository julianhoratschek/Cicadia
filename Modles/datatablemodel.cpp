#include "datatablemodel.h"

DataTableModel::DataTableModel(QObject *parent, CCDataBase *_dataBase)
    : QAbstractTableModel(parent), dataBase(_dataBase), timeStart(MSecsInDay / 1000), timeInterval(SecsIn10Min), rows(0)
{
}


QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
        return dataTable[section]->getName();
    else if(orientation == Qt::Vertical)
        return QTime::fromMSecsSinceStartOfDay((timeStart * 1000 + section * timeInterval * 1000) % MSecsInDay).toString("hh:mm");

    return QVariant();
}


int DataTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return rows;
}


int DataTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return dataTable.count();
}


QVariant DataTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto        dataset = dataTable[index.column()];

    if(role == Qt::DisplayRole) {
        int         add = ((timeStart + index.row() * timeInterval) - dataset->from()) / timeInterval;

        if(add < 0 || dataset->from() + add * timeInterval > dataset->to())
            return "null";
        return *(dataset->begin() + add);
    }
    else if(role == Qt::BackgroundRole)
        return dataset->getColor();

    return QVariant();
}


bool DataTableModel::insertDataset(CCDataSetPtr dataset)
{
    if(!dataset->getData()) {
        CCDataPtr       data(dataBase->selectData(dataset->getDataId()));
        dataset->setData(data);
    }

    if(dataset->getData()->interval != timeInterval)
        return false;

    qint64      t = QDateTime::fromSecsSinceEpoch(dataset->from()).time().msecsSinceStartOfDay() / 1000;

    if(t < timeStart)
        timeStart = t;

    int         size = (t - timeStart) / timeInterval + dataset->size();
    if(size > rows) {
        beginInsertRows(createIndex(0, 0), rows, size);
        rows = size;
        endInsertColumns();
    }


    beginInsertColumns(createIndex(0, 0), dataTable.count(), dataTable.count());
    dataTable << dataset;
    endInsertColumns();

    return true;
}


bool DataTableModel::removeDataset(int column)
{
    beginRemoveColumns(createIndex(0, 0), column, column);
    dataTable.remove(column);
    endRemoveColumns();

    return true;
}
