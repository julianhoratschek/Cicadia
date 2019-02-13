#ifndef ALGORITHMBASE_H
#define ALGORITHMBASE_H

#include "../dataset.h"

class AlgorithmBase
{
public:
    enum AlgorithmType {
        SingleComponentCosinor = 1,
        MultipleComponentCosinor = 1 << 1,
        PopulationCosinor = 1 << 2,
        Cosinor = SingleComponentCosinor | MultipleComponentCosinor | PopulationCosinor,
        Comparison = 1 << 3,
        Histogram = 1 << 4,
        HenningSmoothed = 1 << 5
    };


    AlgorithmBase(AlgorithmType _type = SingleComponentCosinor)
        : type(_type) {}
    virtual ~AlgorithmBase() {}

    virtual CCDataPtr getData() const = 0;
    AlgorithmType getType() { return type; }
    QString getName();

private:
    AlgorithmType       type;
};

enum CCSerialization {
    CCSMagicNumber = 0xC1CAD1A,
    CCSVersion = 1,
    CCSStatisticsTableItem,
    CCSStatisticsTableModel,
    CCSDataTableModel,
    CCSDataTab
};

#endif // ALGORITHMBASE_H
