#include "subjectstreemodel.h"


SubjectsTreeModel::SubjectsTreeModel(QObject *parent, CCDataBase *_dataBase)
    : QAbstractItemModel(parent), dataBase(_dataBase)
{
    root = new SubjectsTreeItem(nullptr);
    root->children.append(new SubjectsTreeItem(root));
    root->children.append(new SubjectsTreeItem(root));

    auto            subjects = dataBase->selectSubjects();

    for(auto subject: subjects) {
        auto        datasets = dataBase->selectDatasets(subject.id);
        auto        node = subject.isGroup ? groupsItem : subjectsItem,
                    newNode = new SubjectsTreeItem(node);

        newNode->dataset = datasets.first();
        newNode->name = subject.name;
        node->children.append(newNode);

        for(int i = 1; i < datasets.count(); i++) {
            newNode->children.append(new SubjectsTreeItem(newNode));
            newNode->children.last()->dataset = datasets[i];
            newNode->children.last()->name = datasets[i]->getName();
        }
    }
}


QVariant SubjectsTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(section) {
    case NameColumn:
        return "Name";
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


QModelIndex SubjectsTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    auto        ptr = static_cast<SubjectsTreeItem*>(parent.internalPointer());

    return createIndex(row, column, ptr->children[row]);
}


QModelIndex SubjectsTreeModel::parent(const QModelIndex &index) const
{
    auto        ptr = static_cast<SubjectsTreeItem*>(index.internalPointer())->parent;

    return createIndex(ptr->parent ? ptr->parent->children.indexOf(ptr) : 0, 0, ptr);
}


int SubjectsTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    return static_cast<SubjectsTreeItem*>(parent.internalPointer())->children.count();
}


int SubjectsTreeModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    return SubjectsTreeModel_ColumnCount;
}


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
            return ptr->dataset->getType();
        }
        break;
    }

    return QVariant();
}


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


Qt::ItemFlags SubjectsTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index); // FIXME: Implement me!
}


bool SubjectsTreeModel::insertDataset(const CCSubject &subject, CCDataSetPtr newSet)
{
    auto        parent = getModelIndex(dataBase->selectDataset(newSet->getParentId()),
                                       subject.isGroup ? groupsItem : subjectsItem);
    auto        parentItem = static_cast<SubjectsTreeItem*>(parent.internalPointer());

    beginInsertRows(parent, parentItem->children.count(), parentItem->children.count());
    parentItem->children.append(new SubjectsTreeItem(parentItem));
    parentItem->children.last()->dataset = newSet;
    parentItem->children.last()->name = newSet->getName();
    endInsertRows();

    return true;
}


bool SubjectsTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}


QModelIndex SubjectsTreeModel::getModelIndex(CCDataSetPtr search, SubjectsTreeItem *ptr) const
{
    if(ptr->dataset == search)
        return createIndex(0, 0, ptr);

    for(int i = 0; i < ptr->children.count(); i++) {
        if(ptr->dataset->getDataId() != search->getDataId())
            continue;

        QModelIndex     ret = getModelIndex(search, ptr->children[i]);
        if(ret.isValid())
            return ret;
    }

    return QModelIndex();
}
