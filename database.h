#ifndef DATABASE_H
#define DATABASE_H

#include "dataset.h"

#include <QSqlDatabase>
#include <QSqlQuery>

#include <QVariant>

#include <QDir>
#include <QFileInfo>
#include <QTextStream>

#include <QDateTime>

#include <QDebug>


struct CCSubject
{
    int         id;
    QString     name;
    bool        isGroup;

    CCSubject(int _id = 0, const QString &_name = "", bool _group = false)
        :id(_id), name(_name), isGroup(_group) {}
    CCSubject(const CCSubject &other)
        : id(other.id), name(other.name), isGroup(other.isGroup) {}
};


#define beginOperation          db.transaction()
#define execOperation(x)        if(!q.exec()){ db.rollback(); return (x);}
#define endOperation            db.commit()


class CCDataBase
{
public:
    CCDataBase();
    ~CCDataBase();

    QSharedPointer<CCDataSet>       importFromFile(const QString &fileName);

    QVector<CCSubject>              selectSubjects();
    CCSubject                       selectSubject(int subjectId);
    QVector<CCDataSetPtr>           selectDatasets(int subjectId);
    CCDataSetPtr                    selectDataset(int datasetId);
    CCDataPtr                       selectData(int dataId);
    QString                         selectStatistics(int datasetId);

    CCDataSetPtr                    insertDataset(CCDataSetPtr dataset, int subjectId);
    CCDataSetPtr                    insertData(CCDataPtr data, int parentId = 0, const QString &suffix = "Raw", const QColor &color = QColor(StdColor), CCDataSet::DataType type = CCDataSet::RawData);
    void                            insertStatistics(int datasetId, const QString &stats);

    int                             updateDataset(const CCDataSet &dataset);

    void                            hideData(CCDataSetPtr dataset, qint64 start, qint64 end);
    void                            deleteDataset(int datasetId);

    void save(const QString &fileName);
    void load(const QString &fileName);

private:
    QSqlDatabase                    db;

    QMap<int, CCDataSetPtr>         datasets;
    QMap<int, CCDataPtr>            dataContainer;

    CCDataSetPtr                    addDataset(const QSqlQuery &q);
    QDateTime                       cleanTime(const QDateTime &dt) const;
    void collectAndDeleteDatasets(int datasetId, QVector<int> *dataIds);
};

#endif // DATABASE_H
