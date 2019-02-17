#include "database.h"
#include "database.h"


CCDataBase::CCDataBase()
{
    QDir::setCurrent(QDir::homePath());

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QDir::currentPath() + "/cicadia.db");
    db.open();

    db.exec("create table if not exists `Subject` (`ID` INTEGER primary key, `Name` TEXT)");
    db.exec("create table if not exists `SubjectMeta` (`Subject_ID` INTEGER, `First_Name` TEXT, `Last_Name` TEXT, `Gender` BOOLEAN, `Age` INTEGER)");
    db.exec("create table if not exists `GroupRelations` (`Group_ID` INTEGER, `Subject_ID` INTEGER)");
    db.exec("create table if not exists `Data` (`ID` INTEGER, `Time` DATETIME, `Value` REAL, `Used` BOOLEAN)");
    db.exec("create table if not exists `Dataset` (`ID` INTEGER primary key, `Parent_ID` INTEGER, `Data_ID` INTEGER, `Start` DATETIME, `End` DATETIME, `Suffix` TEXT, `Color` INTEGER, `Type` INTEGER)");
    db.exec("create table if not exists `DatasetRelations` (`Subject_ID` INTEGER, `Dataset_ID` INTEGER)");
    db.exec("create table if not exists `Statistics` (`Dataset_ID` INTEGER, `Data` TEXT)");
}


CCDataBase::~CCDataBase()
{
    datasets.clear();
    dataContainer.clear();
    db.close();
}


CCDataSetPtr CCDataBase::importFromFile(const QString &fileName)
{
    QFileInfo           info(fileName);
    QFile               fl(fileName);
    QTextStream         stream(&fl);
    CCDataPtr           data(new CCData<qint64>(""));


    if(info.suffix() != "csv")
        return {};

    if(!fl.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    // Make sure, CSV-File is generated by 1-Wire
    if(stream.readLine().left(14) != "1-Wire/iButton") {
        fl.close();
        return {};
    }

    // Read iButton-ID as Name for Subject
    data->name = stream.readLine().right(16) + info.baseName();

    for(int lineNum = 3; !stream.atEnd(); lineNum++) {
        QString         line = stream.readLine();

        // Skip unnecessary lines
        if(lineNum < 16)
            continue;

        // Split Columns by "," into l[0] = DateTime, l[1] = "C", l[2] = Temp, (l[3] = Temp floating point)
        QStringList     l = line.split(",");
        QDateTime       dt = QDateTime::fromString(l[0], "dd.MM.yy HH:mm:ss");

        dt = cleanTime(dt);

        // Qt can't handle two-digit years over 1999
        dt = dt.addYears(100);
        //dt.setTimeSpec(Qt::UTC);

        // Circumvents Error in 1-Wire CSV-File
        data->insert(dt.toSecsSinceEpoch(),
                     l.size() == 4 ?
                         QString( l[2] + "." + l[3] ).toDouble()
                       : l[2].toDouble());
    }

    fl.close();

    //autoInsertMarkers(data); // TODO

    return insertData(data);
}


QVector<CCSubject> CCDataBase::selectSubjects()
{
    QVector<CCSubject>      ret;
    QSqlQuery               q(db);

    q.prepare("select `ID`, `Name`, count(`Group_ID`) as `isGroup` from `Subject` left join `GroupRelations` on `ID` = `Subject_ID` group by `ID`");
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

CCSubject CCDataBase::selectSubject(int subjectId)
{
    QSqlQuery               q(db);

    q.prepare("select `ID`, `Name`, count(`Group_ID`) as `isGroup` from `Subject` left join `GroupRelations` on `ID` = `Subject_ID` where `ID` = ? group by `ID`");
    q.addBindValue(subjectId);
    if(!q.exec() || !q.next())
        return {};

    return CCSubject(q.value("ID").toInt(), q.value("Name").toString(), q.value("isGroup").toBool());
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


CCDataPtr CCDataBase::selectData(int dataId)
{
    if(dataContainer.contains(dataId))
        return dataContainer[dataId];

    QSqlQuery               q(db);

    q.prepare("select `Name` from `Subject` where `ID` = ?");
    q.addBindValue(dataId);
    if(!q.exec() || !q.next())
        return nullptr;

    CCDataPtr               dt(new CCData<qint64>(q.value("Name").toString()));

    q.prepare("select `Time`, `Value`, `Used` from `Data` where `ID` = ?");
    q.addBindValue(dataId);
    if(!q.exec())
        return nullptr;


    while(q.next())
        dt->insert(q.value("Time").toLongLong(), q.value("Value").toDouble(), q.value("Used").toBool());

    dt->interval = (dt->internal.lastKey() - dt->internal.firstKey()) / dt->internal.size();
    int         diff = dt->interval % 10;
    if(diff != 0)
        dt->interval += 10 - diff;
    dataContainer.insert(dataId, dt);

    return dt;
}

QString CCDataBase::selectStatistics(int datasetId)
{
    QSqlQuery       q(db);

    q.prepare("select `Data` from `Statistics` where `Dataset_ID` = ?");
    q.addBindValue(datasetId);
    if(!q.exec() || !q.next())
        return "";
    return q.value(0).toString();
}


QSharedPointer<CCDataSet> CCDataBase::insertDataset(QSharedPointer<CCDataSet> dataset, int subjectId)
{
    QSqlQuery           q(db);

    beginOperation;

    q.prepare("insert into `Dataset` (`Parent_ID`, `Data_ID`, `Start`, `End`, `Suffix`, `Color`, `Type`) values (?, ?, ?, ?, ?, ?, ?)");
    q.addBindValue(dataset->getParentId());
    q.addBindValue(dataset->getDataId());
    q.addBindValue(dataset->from());
    q.addBindValue(dataset->to());
    q.addBindValue(dataset->getSuffix());
    q.addBindValue(dataset->getColor().rgb());
    q.addBindValue(dataset->getType());
    execOperation(nullptr)

    dataset->setId(q.lastInsertId().toInt());

    q.prepare("insert into `DatasetRelations` (`Subject_ID`, `Dataset_ID`) values (?, ?)");
    q.addBindValue(subjectId);
    q.addBindValue(dataset->getId());
    execOperation(nullptr)

    endOperation;

    return dataset;
}


QSharedPointer<CCDataSet> CCDataBase::insertData(CCDataPtr data, int parentId, const QString &suffix, const QColor &color, CCDataSet::DataType type)
{
    beginOperation;

    QSqlQuery           q(db);

    q.prepare("insert into `Dataset` (`Parent_ID`, `Start`, `End`, `Suffix`, `Color`, `Type`) values (:pid, :start, :end, :suffix, :color, :type)");
    q.bindValue(":pid", parentId);
    q.bindValue(":start", data->internal.firstKey());
    q.bindValue(":end", data->internal.lastKey());
    q.bindValue(":suffix", suffix);
    q.bindValue(":color", color.rgb());
    q.bindValue(":type", type);
    execOperation(nullptr)

    int                 id = q.lastInsertId().toInt();

    q.prepare("insert into `Subject` (`ID`, `Name`) values (?, ?)");
    q.addBindValue(id);
    q.addBindValue(data->name);
    execOperation(nullptr)

    q.prepare("insert into `DatasetRelations` (`Subject_ID`, `Dataset_ID`) values (?, ?)");
    q.addBindValue(id);
    q.addBindValue(id);
    execOperation(nullptr)

    for(auto it = data->internal.begin(); it != data->internal.end(); it++) {
        q.prepare("insert into `Data` (`ID`, `Time`, `Value`, `Used`) values (?, ?, ?, 1)");
        q.addBindValue(id);
        q.addBindValue(it.key());
        q.addBindValue(it.value().value);
       execOperation(nullptr)
    }

    q.prepare("update `Dataset` set `Data_ID` = ? where `ID` = ?");
    q.addBindValue(id);
    q.addBindValue(id);
    execOperation(nullptr)

    endOperation;

    return selectDataset(id);
}

void CCDataBase::insertStatistics(int datasetId, const QString &stats)
{
    QSqlQuery       q(db);

    q.prepare("insert into `Statistics` (`Dataset_ID`, `Data`) values (?, ?)");
    q.addBindValue(datasetId);
    q.addBindValue(stats);

    q.exec();
}


int CCDataBase::updateDataset(const CCDataSet &dataset)
{
    QSqlQuery               q(db);

    q.prepare("update `Dataset` set `Suffix` = ?, `Color` = ? where `ID` = ?");
    q.addBindValue(dataset.getSuffix());
    q.addBindValue(dataset.getColor().rgb());
    q.addBindValue(dataset.getId());
    if(!q.exec())
        return false;

    return true;
}

void CCDataBase::hideData(CCDataSetPtr dataset, qint64 start, qint64 end)
{
    QSqlQuery               q(db);

    q.prepare("update `Data` set `Used` = 0 where `ID` = ? and `Time` >= ? and `Time` <= ?");
    q.addBindValue(dataset->getDataId());
    q.addBindValue(start);
    q.addBindValue(end);
    if(!q.exec())
        return;

    if(dataContainer.contains(dataset->getDataId())) {
        for(qint64 i = start; i <= end; i += dataset->getData()->interval)
            dataContainer[dataset->getDataId()]->setUsed(i, false);
    }
}

void CCDataBase::collectAndDeleteDatasets(int datasetId, QVector<int> *dataIds)
{
    QSqlQuery                   q(db);

    q.prepare("select `ID`, `Data_ID` from `Dataset` where `Parent_ID` = ?");
    q.addBindValue(datasetId);
    q.exec();

    while(q.next()) {
        collectAndDeleteDatasets(q.value("ID").toInt(), dataIds);
        if(!dataIds->contains(q.value("Data_ID").toInt()))
            dataIds->append(q.value("Data_ID").toInt());
    }

    q.prepare("delete from `Dataset` where `ID` = ?");
    q.addBindValue(datasetId);
    q.exec();
    q.prepare("delete from `DatasetRelations` where `Dataset_ID` = ?");
    q.addBindValue(datasetId);
    q.exec();
    q.prepare("delete from `Statistics` where `Dataset_ID` = ?");
    q.addBindValue(datasetId);
    q.exec();

    if(datasets.contains(datasetId))
        datasets.remove(datasetId);
}

void CCDataBase::deleteDataset(int datasetId)
{
    QSqlQuery                   q(db);
    QVector<int>                dataIds;

    collectAndDeleteDatasets(datasetId, &dataIds);

    for(auto it: dataIds) {
        q.prepare("select `ID` from `DataSet` where `Data_ID` = ?");
        q.addBindValue(it);
        q.exec();
        if(q.size() < 1) {
            q.prepare("delete from `Data` where `ID` = ?");
            q.addBindValue(it);
            q.exec();
            q.prepare("delete from `Subject` where `ID` = ?");
            q.addBindValue(it);
            q.exec();
            q.prepare("delete from `SubjectMeta` where `Subject_ID` = ?");
            q.addBindValue(it);
            q.exec();
            q.prepare("delete from `GroupRelations` where `Subject_ID` = ?");
            q.addBindValue(it);
            q.exec();

            if(dataContainer.contains(it))
                dataContainer.remove(it);
        }
    }

}


void CCDataBase::save(const QString &fileName)
{
    QFile::copy(db.hostName(), fileName);
    load(fileName);
}


void CCDataBase::load(const QString &fileName)
{
    datasets.clear();
    dataContainer.clear();

    db.close();
    db.setHostName(fileName);
    db.open();
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


QDateTime CCDataBase::cleanTime(const QDateTime &dt) const
{
    int         mindiff = dt.time().minute() % 10;

    if(mindiff != 0) {
        if(mindiff < 5)
            return dt.addSecs(mindiff * -60);
        return dt.addSecs(600 - (mindiff * 60));
    }

    return dt;
}























