#ifndef DATATABLEMODEL_H
#define DATATABLEMODEL_H

#include "../database.h"

#include <QDateTime>

#include <QAbstractTableModel>

class DataTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DataTableModel(QObject *parent = nullptr, CCDataBase *_dataBase = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertDataset(QSharedPointer<CCDataSet> dataset);

    // Remove data:
    bool removeDataset(int column);

private:
    CCDataBase                  *dataBase;
    QVector<CCDataSetPtr>       dataTable;
    qint64                      timeStart, timeInterval;
    int                         rows;
};

#endif // DATATABLEMODEL_H
