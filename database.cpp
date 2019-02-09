#include "database.h"


CCDataBase::CCDataBase()
{
    db = QSqlDatabase::addDatabase("SQLITE3");
    db.setDatabaseName("cicadia.db");
    db.open();

    db.exec("create table if not exists `Subject` (`ID` integer primary key, `Name` text)");
    db.exec("create table if not exists `SubjectMeta` (`Subject_ID` integer, `First_Name` text, `Last_Name` text, `Gender` boolean, `Age` integer)");
    db.exec("create table if not exists `GroupRelations` (`Group_ID` integer, `Subject_ID` integer)");
    db.exec("create table if not exists `Data` (`ID` integer, `Time` datetime, `Value` real, `Used` boolean)");
    db.exec("create table if not exists `Dataset` (`ID` integer primary key auto_increment, `Parent_ID` integer, `Data_ID` integer, `Start` datetime, `End` datetime, `Suffix` text, `Color` integer, `Type` integer)");
    db.exec("create table if not exists `DatasetRelations` (`Subject_ID` integer, `Dataset_ID` integer)");
}


CCDataBase::~CCDataBase()
{
    db.close();
}


QVector<CCSubject> CCDataBase::selectSubjects()
{
    QVector<CCSubject>      ret;
    QSqlQuery               q(db);

    q.prepare("select `ID`, `Name`, count(`Group_ID`) as `isGroup` from `Subject` join `GroupRelations` on `ID` = `Subject_ID` group by `Group_ID`");
    if(!q.exec())
        return {};
    while(q.next()) {
        CCSubject   s(  q.value("ID").toInt(),
                        q.value("Name").toString(),
                        q.value("isGroup").toBool());
        ret << s;
    }

    return ret;
}


QVector<QSharedPointer<CCDataSet> > CCDataBase::selectDatasets(int subjectId)
{
    QVector<CCDataSetPtr>       ret;
    QSqlQuery                   q(db);

    q.prepare("select `Dataset`.`ID` as `DsID`, `Parent_ID`, `Data_ID`, `Start`, `End`, `Suffix`, `Color`, `Type` from `Dataset`, `DatasetRelations` where `Subject_ID` = ? and `Dataset`.`ID` = `Dataset_ID`");
    q.addBindValue(subjectId);
    if(!q.exec())
        return {};

    while(q.next())
        ret << addDataset(q);

    return ret;
}


QSharedPointer<CCDataSet> CCDataBase::selectDataset(int datasetId)
{
    QSqlQuery               q(db);

    if(datasets.contains(datasetId))
        return datasets[datasetId];

    q.prepare("select `ID` as `DsID`, `Parent_ID`, `Data_ID`, `Start`, `End`, `Suffix`, `Color`, `Type` from `Dataset` where `ID` = ?");
    q.addBindValue(datasetId);
    if(!q.exec() || !q.next())
        return {};

    return addDataset(q);
}

QSharedPointer<CCData> CCDataBase::selectData(int dataId)
{
    if(data.contains(dataId))
        return data[dataId];

    QSqlQuery               q(db);

    q.prepare("select `Name` from `Subject` where `ID` = ?");
    q.addBindValue(dataId);
    if(!q.exec() || !q.next())
        return nullptr;

    CCDataPtr               dt(new CCData(q.value("Name").toString()));

    q.prepare("select `Time`, `Value`, `Used` from `Data` where `ID` = ?");
    q.addBindValue(dataId);
    if(!q.exec())
        return nullptr;

    qint64      mean = 0;
    while(q.next()) {
        mean += q.value("Time").toLongLong();
        if(q.value("Used").toBool())
            dt->internal.insert(q.value("Time").toLongLong(), q.value("Value").toDouble());
    }

    dt->interval = mean / dt->internal.size();
    data.insert(dataId, dt);

    return dt;
}


int CCDataBase::updateDataset(const CCDataSet &dataset)
{
    QSqlQuery               q(db);

    q.prepare("update `Dataset` set `Suffix` = ?, `Color` = ? where `ID` = ?");
    q.addBindValue(dataset.getSuffix());
    q.addBindValue(dataset.getColor());
    q.addBindValue(dataset.getId());
    if(!q.exec())
        return false;

    return true;
}


QSharedPointer<CCDataSet> CCDataBase::addDataset(const QSqlQuery &q)
{
    int                 id = q.value("DsID").toInt();

    if(!datasets.contains(id)) {
        CCDataSetPtr        ds(new CCDataSet(id, q.value("Parent_ID").toInt()));

        ds->setDataId(q.value("Data_ID").toInt());
        ds->setRange(q.value("Start").toLongLong(), q.value("End").toLongLong());
        ds->setSuffix(q.value("Suffix").toString());
        ds->setColor(QRgb(q.value("Color").toInt()));
        ds->setType(q.value("Type").toInt());

        datasets.insert(id, ds);
    }

    return datasets[id];
}























