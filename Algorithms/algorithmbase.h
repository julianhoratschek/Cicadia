#ifndef ALGORITHMBASE_H
#define ALGORITHMBASE_H

#include <QStringList>

namespace AlgorithmType {
    enum Name {
        SingleComponentCosinor = 1,
        MultipleComponentCosinor = 1 << 1,
        PopulationCosinor = 1 << 2,
        Cosinor = SingleComponentCosinor | MultipleComponentCosinor | PopulationCosinor,
        Comparison = 1 << 3,
        Histogram = 1 << 4,
        HenningSmoothed = 1 << 5
    };
}

struct AlgorithmData
{
    double      mean;

    AlgorithmData() :
        mean(0) {}
    AlgorithmData(const AlgorithmData &other) :
        mean(other.mean) {}
    virtual ~AlgorithmData() {}

    virtual QStringList toString() const {
        return {"Mean: ", QString::number(mean)};
    }
};

template<typename DataPtr>
class AlgorithmBase
{
public:
    AlgorithmBase(AlgorithmType::Name _type = AlgorithmType::SingleComponentCosinor)
        : type(_type), data(nullptr) {}
    virtual ~AlgorithmBase() {}

    virtual DataPtr         getData() const = 0;
    AlgorithmType::Name     getType() { return type; }
    QString                 getName() {
        switch((int)type) {
        case AlgorithmType::SingleComponentCosinor:
            return "Single Component Cosionr";
        case AlgorithmType::MultipleComponentCosinor:
            return "Multiple Component Cosinor";
        case AlgorithmType::PopulationCosinor:
            return "Population Cosinor";
        case AlgorithmType::Comparison:
            return "Cosinor Comparison";
        case AlgorithmType::Histogram:
            return "Histogram";
        case AlgorithmType::HenningSmoothed:
            return "Henning Window Smoothed";
        }

        return "Algorithm";
    }

protected:
    AlgorithmData               *data;

private:
    AlgorithmType::Name         type;

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
