#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include "../dataset.h"

/**
 * @brief The Histogram class
 */
class Histogram : public AlgorithmBase<CCDoubleDataPtr>
{

public:

    // Public Methods

    Histogram(CCDataSetPtr &dataset, int classCount);

    CCDoubleDataPtr         getData() const override { return dd; }
    inline double           stepWidth() const { return steps; }

private:

    // Private Member

    CCDoubleDataPtr         dd;
    double                  steps;
};

#endif // HISTOGRAMM_H
