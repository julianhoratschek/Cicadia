#ifndef DATASET_H
#define DATASET_H

#include    <QVector>
#include    <QMap>

#include    <QString>
#include    <QColor>

#include    <QSharedPointer>

#define EArrayXld       Eigen::Array<QCDDouble, Eigen::Dynamic, 1>
#define EMatrix3ld      Eigen::Matrix<QCDDouble, 3, 3>
#define EVector3ld      Eigen::Matrix<QCDDouble, 3, 1>

#define MSecsInDay     86400
#define SecsIn10Min    600

struct CCData {
    int                         interval;
    QString                     name;
    QMap<qint64, double>        internal;

    CCData(const QString &_name)
        :interval(SecsIn10Min), name(_name) {}
};

#define CCDataPtr         QSharedPointer<CCData>
#define CCDataIterator    QMap<qint64, double>::iterator

#define StdColor        4279941833

class CCDataSet : public QObject
{
    Q_OBJECT

public:

    enum DataType {
        RawData = 0,
        DerivedData = 1,
        ProcessedData = 1 << 20
    };

    CCDataSet(int _id = 0, int _parentId = 0)
        :   suffix(""), color(QColor(StdColor)), data(nullptr), fromTime(0),
            toTime(0), id(_id), parentId(_parentId), dataId(0), type(RawData) {}
    CCDataSet(const CCDataSet &other)
        :   suffix(other.suffix), color(other.color), data(other.data), fromTime(other.fromTime),
            toTime(other.toTime), id(0), parentId(other.id), dataId(other.dataId), type(DerivedData)
            {}
    ~CCDataSet()
            { data.clear(); }

    inline void                 setSuffix(const QString &_suffix)
                                    { suffix = _suffix; }
    inline QString              getSuffix() const
                                    { return suffix; }
    inline QString              getName() const
                                    { return data ? data->name + suffix : suffix; }
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

    inline int                  size() const
                                    { return data ? (toTime - fromTime) / data->interval : 0; }

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
};

#define CCDataSetPtr      QSharedPointer<CCDataSet>

#endif // DATASET_H
