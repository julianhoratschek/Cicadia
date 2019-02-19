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


/**
 * @brief The CCSubject struct holds Information an a Subject-Object, which mainly describes a wider Informationholder for a Data-Element. Subjects always have the SAME ID as a corresponding Base-Data-Element in the Database.
 */
struct CCSubject
{
    // Public Members
    int             id;
    QString         name;
    bool            isGroup;

    CCSubject(int _id = 0, const QString &_name = "", bool _group = false) : id(_id), name(_name), isGroup(_group) {}
    CCSubject(const CCSubject &other) : id(other.id), name(other.name), isGroup(other.isGroup) {}
};


// Convenience Defines for Database Transactions

#define beginOperation          db.transaction()
#define execOperation(x)        if(!q.exec()){ db.rollback(); return (x);}
#define endOperation            db.commit()


/**
 * @brief The CCDataBase class Holds manipulation Functions for a Cicadia Database
 */
class CCDataBase
{
public:

    // Public Methods

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
    CCDataSetPtr                    insertData(CCDataPtr const &data, int parentId = 0, const QString &suffix = "Raw", const QColor &color = QColor(StdColor), DataSetType type = DataSetType::RawData);
    void                            insertStatistics(int datasetId, const QString &stats);

    int                             updateDataset(const CCDataSet &dataset);

    void                            hideData(CCDataSetPtr const &dataset, qint64 start, qint64 end);
    void                            deleteDataset(int datasetId);

    bool                            save(const QString &fileName);
    bool                            load(const QString &fileName);

private:

    // Private Members

    QSqlDatabase                    db;

    QMap<int, CCDataSetPtr>         datasets;
    QMap<int, CCDataPtr>            dataContainer;

    // Private Methods

    CCDataSetPtr                    addDataset(const QSqlQuery &q);
    QDateTime                       cleanTime(const QDateTime &dt) const;
    void                            collectAndDeleteDatasets(int datasetId, QVector<int> *dataIds);
};

#endif // DATABASE_H
