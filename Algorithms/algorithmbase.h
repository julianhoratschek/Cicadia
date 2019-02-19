#ifndef ALGORITHMBASE_H
#define ALGORITHMBASE_H

#include <QString>
#include <QStringList>

/**
 * @brief The AlgorithmType enum describes the Algorithm used and can be used to
 * determine to which class a AlgorithmBase-Pointer shall be casted
 */
enum class AlgorithmType : qint64 {
    SingleComponentCosinor = 1,
    MultipleComponentCosinor = 1 << 1,
    PopulationCosinor = 1 << 2,
    Cosinor = SingleComponentCosinor | MultipleComponentCosinor | PopulationCosinor,
    Comparison = 1 << 3,
    Histogram = 1 << 4,
    HenningSmoothed = 1 << 5
};


/**
 * @brief The AlgorithmData struct holds statistical Information about a current Algorithm Class.
 * It is not mandatory for a Class to function.
 */
struct AlgorithmData
{
    // Public Members

    double      mean;

    // Public Methods

    AlgorithmData() : mean(0) {}
    AlgorithmData(const AlgorithmData &other) : mean(other.mean) {}
    virtual ~AlgorithmData() {}

    virtual void            load(const QString &s) = 0;
    virtual QString         save() const = 0;

    virtual QStringList     toStringList() const { return {"Mean: ", QString::number(mean)}; }
};


/**
 *  @brief The AlgorithmBase template class is used as a Base for all Data-manipulation Classes.
 * <DataPtr> should be either double or qint64.
 */
template<typename DataPtr>
class AlgorithmBase
{
public:

    // Public Methods

    AlgorithmBase(AlgorithmType _type = AlgorithmType::SingleComponentCosinor) : type(_type), data(nullptr) {}
    virtual ~AlgorithmBase() {}

    AlgorithmType           getType() { return type; }
    QString                 getName() {
        switch(type) {
        case AlgorithmType::SingleComponentCosinor:
            return "Single Component Cosionor";
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

    // Virtual Public Methods

    virtual DataPtr         getData() const = 0;

protected:
    /**
     * @brief data holds a Pointer to the Statistics-relevant derived Pointer to a child-Class of AlgorithmData
     */
    AlgorithmData           *data;

private:

    /**
     * @brief type Holds the Type of the current Child-Class of AlgorithmBase
     */
    AlgorithmType           type;

};


/**
 * @brief The CCSerialization enum is relevant for Storing and serializing Data.
 */
enum CCSerialization : qint32 {
    CCSMagicNumber = 0xC1CAD1A,
    CCSVersion = 2,
    CCSStatisticsTableItem,
    CCSStatisticsTableModel,
    CCSDataTableModel,
    CCSDataTab,
    CCSDataTabOptions
};

#endif // ALGORITHMBASE_H
