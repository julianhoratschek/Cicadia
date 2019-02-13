#include "datatablemodel.h"

DataTableModel::DataTableModel(QObject *parent, CCDataBase *_dataBase)
    : QAbstractTableModel(parent), dataBase(_dataBase), timeStart(SecsInDay), timeInterval(SecsIn10Min), rows(0)
{
}


QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal)
            return dataTable[section]->getName();
        else if(orientation == Qt::Vertical)
            return QTime::fromMSecsSinceStartOfDay((timeStart * 1000 + section * timeInterval * 1000) % (SecsInDay * 1000)).toString("hh:mm");
    }

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
    int         add = ((timeStart + index.row() * timeInterval) -
                       QDateTime::fromSecsSinceEpoch(dataset->from()).time().msecsSinceStartOfDay() / 1000)
                        / timeInterval;
    bool        oob = add < 0 || dataset->from() + add * timeInterval > dataset->to();

    if(role == Qt::DisplayRole || role == KeyRole) {
        if(oob)
            return "null";
        return role == Qt::DisplayRole ? (dataset->begin() + add).value().value : (dataset->begin() + add).key();
    }
    else if(role == Qt::BackgroundRole)
        return (oob || !(dataset->begin() + add).value().used) ? dataset->getColor().lighter() : dataset->getColor();
    else if(role == UsedRole)
        return !oob && (dataset->begin() + add).value().used;

    return QVariant();
}


QSharedPointer<CCDataSet> DataTableModel::getDataset(int column) const
{
    return dataTable[column];
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
        beginInsertRows(QModelIndex(), rows, size);
        rows = size;
        endInsertRows();
    }


    beginInsertColumns(QModelIndex(), dataTable.count(), dataTable.count());
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

void DataTableModel::save(QDataStream &stream)
{
    stream << CCSerialization::CCSDataTableModel;
    stream << (quint32)rows << timeStart << timeInterval << (quint32)dataTable.size();
    for(int i=0;i<dataTable.count();i++)
        stream << (quint32)dataTable[i]->getId();
}

void DataTableModel::load(QDataStream &stream)
{
    quint32     s;

    dataTable.clear();

    stream >> s;
    rows = s;
    stream >> timeStart;
    stream >> timeInterval;
    stream >> s;
    for(quint32 i = 0; i < s; i++) {
        quint32     id;
        stream >> id;
        insertDataset(dataBase->selectDataset(id));
    }
}
