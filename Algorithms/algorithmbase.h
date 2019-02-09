#ifndef ALGORITHMBASE_H
#define ALGORITHMBASE_H


class AlgorithmBase
{
public:
    enum AlgorithmType {
        SingleComponentCosinor = 1,
        MultipleComponentCosinor = 1 << 1,
        PopulationCosinor = 1 << 2,
        Cosinor = SingleComponentCosinor | MultipleComponentCosinor | PopulationCosinor,
        Comparison = 1 << 3,
        Histogram = 1 << 4,
        HenningSmoothed = 1 << 5
    };


    AlgorithmBase();
};

#endif // ALGORITHMBASE_H
