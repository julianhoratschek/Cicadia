#ifndef DATATABLEMODEL_H
#define DATATABLEMODEL_H

#include "../database.h"

#include <QDataStream>
#include <QTextStream>

#include <QAbstractTableModel>


// Define own Qt::Roles

#define KeyRole     0x0100
#define UsedRole    0x0200


/**
 * @brief The DataTableModel class
 */
class DataTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    // Public Methods

    explicit DataTableModel(QObject *parent = nullptr, CCDataBase *_dataBase = nullptr);

    // Header:
    QVariant                headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int                     rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int                     columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant                data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    CCDataSetPtr            getDataset(int column) const;
    CCDataBase              *getDatabase() const;

    // Add data:
    bool                    insertDataset(const QSharedPointer<CCDataSet> &dataset);


    void                    save(QDataStream &stream);
    void                    load(QDataStream &stream);

signals:
    void                    datasetRemoved(int column);

public slots:
    void                    removeDataset(int id);

private:
    CCDataBase              *dataBase;
    QVector<CCDataSetPtr>   dataTable;
    qint64                  timeStart, timeInterval;
    int                     rows;
};

#endif // DATATABLEMODEL_H
