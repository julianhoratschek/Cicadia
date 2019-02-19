#include "datatablemodel.h"


DataTableModel::DataTableModel(QObject *parent, CCDataBase *_dataBase) : QAbstractTableModel(parent), dataBase(_dataBase), timeStart(SecsInDay), timeInterval(SecsIn10Min), rows(0)
{
}


/**
 * @brief DataTableModel::headerData
 * @param section
 * @param orientation
 * @param role
 * @return
 */
QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal)
            return dataTable[section]->getName();
        if(orientation == Qt::Vertical)
            return QTime::fromMSecsSinceStartOfDay((timeStart * 1000 + section * timeInterval * 1000) % (SecsInDay * 1000)).toString("hh:mm");
    }
    else if(role == Qt::DecorationRole && orientation == Qt::Horizontal)
        return dataTable[section]->getColor();

    return QVariant();
}


/**
 * @brief DataTableModel::rowCount
 * @param parent
 * @return
 */
int DataTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return rows;
}


/**
 * @brief DataTableModel::columnCount
 * @param parent
 * @return
 */
int DataTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return dataTable.count();
}


/**
 * @brief DataTableModel::data
 * @param index
 * @param role
 * @return
 */
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

    if(role == Qt::BackgroundRole)
        return (oob || !(dataset->begin() + add).value().used) ? QColor(UnusedColor) : dataset->getColor();

    if(role == Qt::ToolTipRole)
        return dataset->getName();

    if(role == UsedRole)
        return !oob && (dataset->begin() + add).value().used;

    return QVariant();
}


/**
 * @brief DataTableModel::getDataset
 * @param column
 * @return
 */
QSharedPointer<CCDataSet> DataTableModel::getDataset(int column) const
{
    return dataTable[column];
}


/**
 * @brief DataTableModel::getDatabase
 * @return
 */
CCDataBase *DataTableModel::getDatabase() const
{
    return dataBase;
}


/**
 * @brief DataTableModel::insertDataset
 * @param dataset
 * @return
 */
bool DataTableModel::insertDataset(CCDataSetPtr const &dataset)
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

    qint64      size = (t - timeStart) / timeInterval + dataset->size();
    if(size > rows) {
        beginInsertRows(QModelIndex(), rows, size-1);
        rows = size;
        endInsertRows();
    }


    beginInsertColumns(QModelIndex(), dataTable.count(), dataTable.count());
    dataTable << dataset;
    endInsertColumns();

    return true;
}


/**
 * @brief DataTableModel::removeDataset
 * @param id
 */
void DataTableModel::removeDataset(int id)
{
    for(int i = 0; i < dataTable.count(); i++)
        if(dataTable[i]->getId() == id) {
            beginRemoveColumns(QModelIndex(), i, i);
            dataTable.remove(i);
            endRemoveColumns();

            emit datasetRemoved(i);
        }
}


/**
 * @brief DataTableModel::save
 * @param stream
 */
void DataTableModel::save(QDataStream &stream)
{
    stream << CCSerialization::CCSDataTableModel;
    stream << static_cast<qint32>(timeStart) << static_cast<qint32>(timeInterval) << static_cast<qint32>(dataTable.size());
    for(int i = 0; i < dataTable.count(); i++)
        stream << static_cast<qint32>(dataTable[i]->getId());
}


/**
 * @brief DataTableModel::load
 * @param stream
 */
void DataTableModel::load(QDataStream &stream)
{
    qint32     s;

    dataTable.clear();

    stream >> s;
    timeStart = s;
    stream >> s;
    timeInterval = s;
    stream >> s;

    qint32     id;
    for(qint32 i = 0; i < s; i++) {
        stream >> id;
        insertDataset(dataBase->selectDataset(id));
    }
}
