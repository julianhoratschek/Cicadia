#include "dataset.h"


DataSetType operator&(const DataSetType &dt1, const DataSetType &dt2) {
    return static_cast<DataSetType>(static_cast<qint32>(dt1) & static_cast<qint32>(dt2));
}

DataSetType operator|(const DataSetType &dt1, const DataSetType &dt2) {
    return static_cast<DataSetType>(static_cast<qint32>(dt1) | static_cast<qint32>(dt2));
}

DataSetType operator|(const DataSetType &dt, const AlgorithmType &at){
    return static_cast<DataSetType>(dt | static_cast<DataSetType>(at));
}
