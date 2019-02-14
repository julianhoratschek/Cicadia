#include "histogram.h"

Histogram::Histogram(QSharedPointer<CCDataSet> &dataset, int classCount)
    : AlgorithmBase<CCDoubleDataPtr>(AlgorithmType::Histogram),
      data(new CCData<double>(dataset->getData()->name))
{
    double              cutoff = 0;
    QVector<double>     dt(dataset->size());

    int i = 0;
    for(auto it = dataset->begin(); it != dataset->end(); it++)
        dt[i++] = it->value;

    std::sort(dt.begin(), dt.end());

    steps = (dt.last() - dt.first()) / classCount;

    cutoff = dt.first();

    for(auto d: dt) {
        if(d >= cutoff) {
            cutoff += steps * (int)(d / cutoff);
            data->insert(cutoff, 0);
        }

        data->at(cutoff)++;
    }
}
