#ifndef DATASET_H
#define DATASET_H

#include "Algorithms/algorithmbase.h"

#include    <QVector>
#include    <QMap>

#include    <QString>
#include    <QColor>

#include    <QSharedPointer>

#define EArrayXld       Eigen::Array<QCDDouble, Eigen::Dynamic, 1>
#define EMatrix3ld      Eigen::Matrix<QCDDouble, 3, 3>
#define EVector3ld      Eigen::Matrix<QCDDouble, 3, 1>

#define SecsInDay      86400
#define SecsIn10Min    600

struct CCDataPoint {
    double          value;
    bool            used;

    CCDataPoint(double v = 0, bool u = true)
        : value(v), used(u) {}
};

//Q_DECLARE_METATYPE(CCDataPoint);

template<class Key>
struct CCData {
    int                         interval;
    QString                     name;
    QMap<Key, CCDataPoint>      internal;

    CCData(const QString &_name)
        :interval(SecsIn10Min), name(_name) {}

    inline void insert(Key t, double v, bool u = true) { internal.insert(t, CCDataPoint(v, u)); }
    inline double &at(Key t) { return internal[t].value; }
    inline bool used(Key t) { return internal[t].used; }
    inline void setUsed(Key t, bool u = true) { if(internal.contains(t)) internal[t].used = u; }
    inline QVector<Key> keys() { return internal.keys().toVector(); }
    inline QVector<double> values() {
        QVector<double> ret(internal.size());
        int i = 0;
        for(auto it: internal)
            ret[i++] = it.value;
        return ret;
    }
};

#define CCDataPtr               QSharedPointer<CCData<qint64>>
#define CCDataIterator          QMap<qint64, CCDataPoint>::iterator
#define CCDoubleDataPtr         QSharedPointer<CCData<double>>
#define CCDoubleDataIterator    QMap<double, CCDataPoint>::iterator

#define StdColor        0xff71b5ed
#define UnusedColor     0xffbcc4d1

class CCDataSet : public QObject
{
    Q_OBJECT

public:

    enum DataType {
        RawData = 1 << 20,
        DerivedData = 1 << 21,
        ProcessedData = 1 << 22
    };

    CCDataSet(int _id = 0, int _parentId = 0)
        :   QObject(), suffix(""), color(QColor(StdColor)), data(nullptr), fromTime(0),
            toTime(0), id(_id), parentId(_parentId), dataId(0), type(RawData), statistics(nullptr) {}
    CCDataSet(const CCDataSet &other)
        :   QObject(), suffix(other.suffix), color(other.color), data(other.data), fromTime(other.fromTime),
            toTime(other.toTime), id(0), parentId(other.id), dataId(other.dataId), type(DerivedData), statistics(nullptr)
            {}
    ~CCDataSet()
            { data.clear(); delete statistics; }

    inline void                 setSuffix(const QString &_suffix)
                                    { suffix = _suffix; }
    inline QString              getSuffix() const
                                    { return suffix; }
    inline QString              getName() const
                                    { return data ? data->name + ": " + suffix : suffix; }
    inline void                 setColor(const QColor &_color)
                                    { color = _color; }
    inline QColor               getColor() const
                                    { return color; }
    inline void                 setData(const CCDataPtr &_data)
                                    { data = _data; }
    inline CCDataPtr            getData() const
                                    { return data; }
    inline void                 setRange(const qint64 &_from, const qint64 &_to)
                                    { fromTime = _from; toTime = _to; }
    inline void                 getRange(qint64 &_from, qint64 &_to) const
                                    { _from = fromTime; _to = toTime; }
    inline qint64               from() const
                                    { return fromTime; }
    inline qint64               to() const
                                    { return toTime; }
    inline void                 setId(int _id)
                                    { id = _id; }
    inline int                  getId() const
                                    { return id; }
    inline int                  getParentId() const
                                    { return parentId; }
    inline void                 setDataId(int _dataId)
                                    { dataId = _dataId; }
    inline int                  getDataId() const
                                    { return dataId; }
    inline void                 setType(int _type)
                                    { type = _type; }
    inline int                  getType() const
                                    { return type; }
    inline bool                 isType(DataType _type)
                                    { return (type & _type) != 0; }
    inline void                 setStatistics(AlgorithmData *_statistics)
                                    { delete statistics; statistics = _statistics;}
    inline AlgorithmData        *getStatistics()
                                    { return statistics; }

    inline int                  size() const
                                    { return data ? (toTime - fromTime) / data->interval + 1 : 0; }

    inline CCDataIterator       begin() const
                                    { return data ? data->internal.lowerBound(fromTime) : nullptr; }
    inline CCDataIterator       end() const
                                    { return data ? data->internal.upperBound(toTime) : nullptr; }


signals:
    void                        selectionChanged(bool selected);

private:
    QString                     suffix;
    QColor                      color;
    CCDataPtr                   data;
    qint64                      fromTime, toTime;
    int                         id, parentId, dataId, type;
    AlgorithmData               *statistics;
};

#define CCDataSetPtr      QSharedPointer<CCDataSet>

CCDataSet::DataType operator|(const CCDataSet::DataType &dt, const AlgorithmType &at);

#endif // DATASET_H
