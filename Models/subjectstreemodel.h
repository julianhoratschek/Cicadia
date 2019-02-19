#ifndef SUBJECTSTREEMODEL_H
#define SUBJECTSTREEMODEL_H

#include <QAbstractItemModel>
#include "../database.h"


/**
 * @brief The SubjectsTreeItem struct
 */
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


/**
 * @brief The SubjectsTreeModel class
 */
class SubjectsTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    // Public Declarations

    enum ColumnNames {
        NameColumn = 0,
        IDColumn,
        TypeColumn
    };

    // Public Methods

    explicit SubjectsTreeModel(QObject *parent = nullptr, CCDataBase *_dataBase = nullptr);
    ~SubjectsTreeModel() override;

    // Header:
    QVariant            headerData(int section, Qt::Orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    QModelIndex         index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex         parent(const QModelIndex &index) const override;

    int                 rowCount(const QModelIndex &parent = QModelIndex()) const override;
#define SubjectsTreeModel_ColumnCount       3
    int                 columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant            data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool                setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags       flags(const QModelIndex& index) const override;

    // Remove data:
    bool                removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    void                deleteDataset(const CCDataSetPtr &deleteSet);

signals:
    void                removedDataset(int id);


public slots:
    void                insertDataset(CCDataSetPtr const &newSet);

private:

    // Private Members

    SubjectsTreeItem    *root;
#define subjectsItem    root->children[0]
#define groupsItem      root->children[1]
    CCDataBase          *dataBase;


    // Private Methods

    QModelIndex         getModelIndex(const QSharedPointer<CCDataSet> &search, SubjectsTreeItem *ptr = nullptr) const;
};

#endif // SUBJECTSTREEMODEL_H
