#ifndef DATATABLEMODEL_H
#define DATATABLEMODEL_H

#include "../database.h"

#include "../Algorithms/algorithmbase.h"

#include <QDataStream>
#include <QTextStream>

#include <QAbstractTableModel>

#define KeyRole     0x0100
#define UsedRole    0x0200

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
    CCDataSetPtr getDataset(int column) const;
    CCDataBase *getDatabase() const;

    // Add data:
    bool insertDataset(QSharedPointer<CCDataSet> dataset);


    void save(QDataStream &stream);
    void load(QDataStream &stream);

signals:
    void datasetRemoved(int column);

public slots:
    void removeDataset(int id);

private:
    CCDataBase                  *dataBase;
    QVector<CCDataSetPtr>       dataTable;
    qint64                      timeStart, timeInterval;
    int                         rows;
};

#endif // DATATABLEMODEL_H
