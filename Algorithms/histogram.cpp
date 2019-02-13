#include "histogram.h"

Histogram::Histogram(const CCDataSet &dataset, int classCount)
    : AlgorithmBase(AlgorithmBase::Histogram), data(new CCData(dataset.getData()->name))
{
    double              cutoff = 0;
    QVector<double>     dt(dataset.size());

    int i = 0;
    for(auto it = dataset.begin(); it != dataset.end(); it++)
        dt[i++] = it->value;

    std::sort(dt.begin(), dt.end());

    steps = (dt.last() - dt.first()) / classCount;

    cutoff = dt.first();

    for(auto d: dt) {
        if(d >= cutoff) {
            cutoff += steps * (d / cutoff);
            data->insert(cutoff, 0);
        }

        data->at(cutoff)++;
    }
}
