#ifndef DATABASE_H
#define DATABASE_H

#include "dataset.h"

#include <QSqlDatabase>
#include <QSqlQuery>

#include <QVariant>


struct CCSubject
{
    int         id;
    QString     name;
    bool        isGroup;

    CCSubject(int _id = 0, const QString &_name = "", bool _group = false)
        :id(_id), name(_name), isGroup(_group) {}
};


class CCDataBase
{
public:
    CCDataBase();
    ~CCDataBase();

    QVector<CCSubject>      selectSubjects();
    QVector<CCDataSetPtr>   selectDatasets(int subjectId);
    CCDataSetPtr            selectDataset(int datasetId);
    CCDataPtr               selectData(int dataId);

    int                     insertData(CCDataPtr data);

    int                     updateDataset(const CCDataSet &dataset);

private:
    QSqlDatabase                    db;

    QMap<int, CCDataSetPtr>         datasets;
    QMap<int, CCDataPtr>            data;

    CCDataSetPtr            addDataset(const QSqlQuery &q);
};

#endif // DATABASE_H
