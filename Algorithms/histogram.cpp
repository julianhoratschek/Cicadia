#include "histogram.h"

/**
 * @brief Histogram::Histogram
 * @param dataset
 * @param classCount
 */
Histogram::Histogram(QSharedPointer<CCDataSet> &dataset, int classCount) : AlgorithmBase<CCDoubleDataPtr>(AlgorithmType::Histogram),
      dd(new CCData<double>(dataset->getData()->name))
{
    double              cutoff = 0;
    QVector<double>     dt(dataset->size());

    int i = 0;
    //for(auto it = dataset->begin(); it != dataset->end(); it++)
    for(auto &it: *dataset)
        dt[i++] = it.value;

    std::sort(dt.begin(), dt.end());

    steps = (dt.last() - dt.first()) / classCount;

    cutoff = dt.first();

    for(auto d: dt) {
        if(d >= cutoff) {
            cutoff += steps * static_cast<int>(d / cutoff);
            dd->insert(cutoff, 0);
        }

        dd->at(cutoff)++;
    }
}
