#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "../dataset.h"

class Histogram : public AlgorithmBase<CCDoubleDataPtr>
{

public:

    // Public Methods

    Histogram(CCDataSetPtr &dataset, int classCount);

    CCDoubleDataPtr         getData() const override { return data; }
    inline double           stepWidth() const { return steps; }

private:
    CCDoubleDataPtr             data;
    double                      steps;
};

#endif // HISTOGRAMM_H
