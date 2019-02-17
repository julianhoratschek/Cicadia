#include "dataset.h"


CCDataSet::DataType operator|(const CCDataSet::DataType &dt, const AlgorithmType &at){
    return static_cast<CCDataSet::DataType>(dt | static_cast<CCDataSet::DataType>(at));
}
