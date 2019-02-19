#include "subjectstreemodel.h"


SubjectsTreeModel::SubjectsTreeModel(QObject *parent, CCDataBase *_dataBase)
    : QAbstractItemModel(parent), dataBase(_dataBase)
{
    root = new SubjectsTreeItem(nullptr);
    root->children.append(new SubjectsTreeItem(root));
    root->children.append(new SubjectsTreeItem(root));

    subjectsItem->name = "Subjects";
    groupsItem->name = "Groups";

    auto            subjects = dataBase->selectSubjects();

    for(auto const &subject: subjects) {
        auto        datasets = dataBase->selectDatasets(subject.id);

        for(auto const &dataset: datasets)
            insertDataset(dataset);
    }
}

SubjectsTreeModel::~SubjectsTreeModel()
{
    delete root;
}


/**
 * @brief SubjectsTreeModel::headerData
 * @param section
 * @param role
 * @return
 */
QVariant SubjectsTreeModel::headerData(int section, Qt::Orientation, int) const
{
    switch(section) {
    case NameColumn:
        return "Name";
    case IDColumn:
        return "ID";
    case TypeColumn:
        return "Type";
    }
    return QVariant();
}


/*bool SubjectsTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}*/


/**
 * @brief SubjectsTreeModel::index
 * @param row
 * @param column
 * @param parent
 * @return
 */
QModelIndex SubjectsTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    SubjectsTreeItem        *ptr;

    if(!parent.isValid())
        ptr = root;
    else
        ptr = static_cast<SubjectsTreeItem*>(parent.internalPointer());

    if(row < ptr->children.count())
        return createIndex(row, column, ptr->children[row]);
    return {};
}


/**
 * @brief SubjectsTreeModel::parent
 * @param index
 * @return
 */
QModelIndex SubjectsTreeModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return {};

    auto        ptr = static_cast<SubjectsTreeItem*>(index.internalPointer())->parent;

    if(ptr != root)
        return createIndex(ptr->parent->children.indexOf(ptr), 0, ptr);
    return {};
}


/**
 * @brief SubjectsTreeModel::rowCount
 * @param parent
 * @return
 */
int SubjectsTreeModel::rowCount(const QModelIndex &parent) const
{
    SubjectsTreeItem    *ptr;
    if (!parent.isValid())
        ptr = root;
    else
        ptr = static_cast<SubjectsTreeItem*>(parent.internalPointer());

    return ptr->children.count();
}


/**
 * @brief SubjectsTreeModel::columnCount
 * @param parent
 * @return
 */
int SubjectsTreeModel::columnCount(const QModelIndex &) const
{
    return SubjectsTreeModel_ColumnCount;
}


/**
 * @brief SubjectsTreeModel::data
 * @param index
 * @param role
 * @return
 */
QVariant SubjectsTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto        ptr = static_cast<SubjectsTreeItem*>(index.internalPointer());

    switch(role) {
    case Qt::DisplayRole:
        switch(index.column()) {
        case NameColumn:
            return ptr->name;
        case IDColumn:
            return ptr->dataset->getId();
        case TypeColumn:
            return static_cast<int>(ptr->dataset->getType());
        }
        break;
    case Qt::BackgroundRole:
        if(ptr->dataset)
            return ptr->dataset->getColor();
    }

    return QVariant();
}


/**
 * @brief SubjectsTreeModel::setData
 * @param index
 * @param value
 * @param role
 * @return
 */
bool SubjectsTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.column() == NameColumn && data(index, role) != value && value != "") {
        auto        ptr = static_cast<SubjectsTreeItem*>(index.internalPointer());

        ptr->dataset->setSuffix(value.toString());
        dataBase->updateDataset(*ptr->dataset);

        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}


/**
 * @brief SubjectsTreeModel::flags
 * @param index
 * @return
 */
Qt::ItemFlags SubjectsTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}


/**
 * @brief SubjectsTreeModel::insertDataset
 * @param newSet
 */
void SubjectsTreeModel::insertDataset(CCDataSetPtr const &newSet)
{
    SubjectsTreeItem        *parentItem;
    CCSubject               s = dataBase->selectSubject(newSet->getDataId());
    auto                    parent = getModelIndex( dataBase->selectDataset(newSet->getParentId()),
                                                    s.isGroup ? groupsItem : subjectsItem);

    if(!parent.isValid())
        parent = createIndex(0, 0, s.isGroup ? groupsItem : subjectsItem);
    parentItem = static_cast<SubjectsTreeItem*>(parent.internalPointer());


    beginInsertRows(parent, parentItem->children.count(), parentItem->children.count());
    parentItem->children.append(new SubjectsTreeItem(parentItem));
    parentItem->children.last()->dataset = newSet;
    parentItem->children.last()->name = s.name + ": " + newSet->getSuffix();
    endInsertRows();
}


/**
 * @brief SubjectsTreeModel::removeRows
 * @param row
 * @param count
 * @param parent
 * @return
 */
bool SubjectsTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    auto        parentItem = static_cast<SubjectsTreeItem*>(parent.internalPointer());

    for(int i=row; i<row+count; i++)
        emit removedDataset(parentItem->children[i]->dataset->getId());

    beginRemoveRows(parent, row, row + count - 1);
    parentItem->children.remove(row, count);
    endRemoveRows();

    return true;
}


/**
 * @brief SubjectsTreeModel::deleteDataset
 * @param deleteSet
 */
void SubjectsTreeModel::deleteDataset(const QSharedPointer<CCDataSet> &deleteSet)
{
    auto                index = getModelIndex(dataBase->selectDataset(deleteSet->getId()), subjectsItem);
    //SubjectsTreeItem    *parentItem = static_cast<SubjectsTreeItem*>(index.parent().internalPointer());

    removeRows(index.row(), 1, index.parent());
    dataBase->deleteDataset(deleteSet->getId());
}


/**
 * @brief SubjectsTreeModel::getModelIndex
 * @param search
 * @param ptr
 * @return
 */
QModelIndex SubjectsTreeModel::getModelIndex(const CCDataSetPtr &search, SubjectsTreeItem *ptr) const
{
    if(!search)
        return {};

    if(ptr->dataset && (ptr->dataset->getId() == search->getId()))
        return createIndex(ptr->parent->children.indexOf(ptr), 0, ptr);

    for(int i = 0; i < ptr->children.count(); i++) {
        QModelIndex     ret = getModelIndex(search, ptr->children[i]);
        if(ret.isValid())
            return ret;
    }

    return {};
}
