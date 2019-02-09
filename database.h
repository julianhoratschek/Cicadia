#ifndef DATABASE_H
#define DATABASE_H

#include "dataset.h"

#include <QSqlDatabase>
#include <QSqlQuery>

#include <QVariant>

#include <QFileInfo>
#include <QTextStream>

#include <QDateTime>


struct CCSubject
{
    int         id;
    QString     name;
    bool        isGroup;

    CCSubject(int _id = 0, const QString &_name = "", bool _group = false)
        :id(_id), name(_name), isGroup(_group) {}
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
    QVector<CCDataSetPtr>           selectDatasets(int subjectId);
    CCDataSetPtr                    selectDataset(int datasetId);
    CCDataPtr                       selectData(int dataId);

    CCDataSetPtr                    insertDataset(CCDataSetPtr dataset, int subjectId);
    CCDataSetPtr                    insertData(CCDataPtr data, int parentId = 0, const QString &suffix = "Raw", const QColor &color = QColor(StdColor), CCDataSet::DataType type = CCDataSet::RawData);

    int                             updateDataset(const CCDataSet &dataset);

private:
    QSqlDatabase                    db;

    QMap<int, CCDataSetPtr>         datasets;
    QMap<int, CCDataPtr>            data;

    CCDataSetPtr                    addDataset(const QSqlQuery &q);
    QDateTime                       cleanTime(const QDateTime &dt) const;
};

#endif // DATABASE_H
