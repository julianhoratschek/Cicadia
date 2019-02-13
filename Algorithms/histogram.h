#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "algorithmbase.h"

class Histogram : public AlgorithmBase
{

public:

    // Public Methods

    Histogram(const CCDataSet &dataset, int classCount);

    CCDataPtr               getData() const override { return data; }
    inline double           stepWidth() const { return steps; }

private:
    CCDataPtr                   data;
    double                      steps;
};

#endif // HISTOGRAMM_H
