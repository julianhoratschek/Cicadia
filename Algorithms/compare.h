#ifndef COMPARE_H
#define COMPARE_H

#include "../dataset.h"
#include "cosinor.h"

struct CompareData : public CosinorData {

};

class Compare : public AlgorithmBase<CCDataPtr>
{
public:
    Compare();
};

#endif // COMPARE_H
