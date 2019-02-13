#include "algorithmbase.h"

/*AlgorithmBase::AlgorithmBase()
{

}*/

QString AlgorithmBase::getName()
{
    switch((int)type) {
    case SingleComponentCosinor:
        return "Single Component Cosionr";
    case MultipleComponentCosinor:
        return "Multiple Component Cosinor";
    case PopulationCosinor:
        return "Population Cosinor";
    case Comparison:
        return "Cosinor Comparison";
    case Histogram:
        return "Histogram";
    case HenningSmoothed:
        return "Henning Window Smoothed";
    }

    return "Algorithm";
}
