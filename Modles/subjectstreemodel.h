#ifndef SUBJECTSTREEMODEL_H
#define SUBJECTSTREEMODEL_H

#include <QAbstractItemModel>
#include "../database.h"

struct SubjectsTreeItem {
    CCDataSetPtr                    dataset;
    QString                         name;
    SubjectsTreeItem                *parent;
    QVector<SubjectsTreeItem*>      children;

    SubjectsTreeItem(SubjectsTreeItem *_parent)
        : parent(_parent) {}
    ~SubjectsTreeItem() {
        qDeleteAll(children);
    }
};


class SubjectsTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    enum ColumnNames {
        NameColumn = 0,
        IDColumn,
        TypeColumn
    };

    explicit SubjectsTreeModel(QObject *parent = nullptr, CCDataBase *_dataBase = nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
#define SubjectsTreeModel_ColumnCount       3
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertDataset(const CCSubject &subject, CCDataSetPtr newSet);

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

private:
    SubjectsTreeItem            *root;
#define subjectsItem            root->children[0]
#define groupsItem              root->children[1]
    CCDataBase                  *dataBase;

    QModelIndex                 getModelIndex(QSharedPointer<CCDataSet> search, SubjectsTreeItem *ptr = nullptr) const;
};

#endif // SUBJECTSTREEMODEL_H
