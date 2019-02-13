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

    for(auto subject: subjects) {
        auto        datasets = dataBase->selectDatasets(subject.id);
        auto        node = subject.isGroup ? groupsItem : subjectsItem,
                    newNode = new SubjectsTreeItem(node);

        newNode->dataset = datasets.first();
        newNode->name = subject.name + "_" + datasets.first()->getSuffix();
        node->children.append(newNode);

        for(int i = 1; i < datasets.count(); i++) {
            newNode->children.append(new SubjectsTreeItem(newNode));
            newNode->children.last()->dataset = datasets[i];
            newNode->children.last()->name = datasets[i]->getName();
        }
    }
}

SubjectsTreeModel::~SubjectsTreeModel()
{
    delete root;
}


QVariant SubjectsTreeModel::headerData(int section, Qt::Orientation, int role) const
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


QModelIndex SubjectsTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    SubjectsTreeItem        *ptr;

    if(!parent.isValid())
        ptr = root;
    else
        ptr = static_cast<SubjectsTreeItem*>(parent.internalPointer());

    if(row < ptr->children.count())
        return createIndex(row, column, ptr->children[row]);
    return QModelIndex();
}


QModelIndex SubjectsTreeModel::parent(const QModelIndex &index) const
{
    if(!index.isValid())
        return QModelIndex();

    auto        ptr = static_cast<SubjectsTreeItem*>(index.internalPointer())->parent;

    if(ptr != root)
        return createIndex(ptr->parent->children.indexOf(ptr), 0, ptr);
    return QModelIndex();
}


int SubjectsTreeModel::rowCount(const QModelIndex &parent) const
{
    SubjectsTreeItem    *ptr;
    if (!parent.isValid())
        ptr = root;
    else
        ptr = static_cast<SubjectsTreeItem*>(parent.internalPointer());

    return ptr->children.count();
}


int SubjectsTreeModel::columnCount(const QModelIndex &parent) const
{
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
    case Qt::BackgroundRole:
        return ptr->dataset->getColor();
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


void SubjectsTreeModel::insertDataset(CCDataSetPtr newSet)
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
    parentItem->children.last()->name = s.name + "_" + newSet->getSuffix();
    endInsertRows();
}


bool SubjectsTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();

    return true;
}


QModelIndex SubjectsTreeModel::getModelIndex(CCDataSetPtr search, SubjectsTreeItem *ptr) const
{
    if(ptr->dataset == search)
        return createIndex(0, 0, ptr);

    for(int i = 0; i < ptr->children.count(); i++) {
        QModelIndex     ret = getModelIndex(search, ptr->children[i]);
        if(ret.isValid())
            return ret;
    }

    return QModelIndex();
}
