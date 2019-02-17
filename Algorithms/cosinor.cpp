#include "cosinor.h"

#include <iostream>

using namespace Eigen;

int Cosinor::runsTestRuns = 5;


void Cosinor::init()
{
    t.resize(getPack()->N);
    Y.resize(getPack()->N);

    int     i = 0;
    for(auto it = dataset->begin(); it != dataset->end(); it++) {
        if(!it.value().used)
            continue;
        t(i) = it.key() / getPack()->tau;
        Y(i++) = it.value().value;
    }
}


Cosinor::Cosinor(const CCDataSetPtr &_dataset, double _timePeriod)
    : AlgorithmBase<CCDataPtr>(), dataset(_dataset)
{
    data = new CosinorData();

    getPack()->tau = _timePeriod;
    getPack()->N = dataset->size();

    init();

    calc();
}


Cosinor::Cosinor(CosinorData *dt, const CCDataSetPtr &_cosinor, const CCDataSetPtr &_parent)
    : AlgorithmBase<CCDataPtr>(), dataset(_parent)
{
    data = dt;

    init();

    Yest.resize(getPack()->N);
    int     i = 0;
    for(auto it = _cosinor->begin(); it != _cosinor->end(); it++) {
        if(!it.value().used)
            continue;
        Yest(i++) = it.value().value;
    }

    beta = getPack()->A * std::cos(getPack()->phi);
    gamma = -getPack()->A * std::sin(getPack()->phi);
}


void Cosinor::recalc(const double &alpha)
{
    mesorCI(alpha);
    acroPhaseCI(alpha);
    amplitudeCI(alpha);
    zeroAmplitude(alpha);
    runsTest(alpha);
}


CCDataPtr Cosinor::getData() const
{
    CCDataPtr       ret(new CCData<qint64>("SCC [T=" + QString::number(getPack()->tau) + "]" + dataset->getData()->name + dataset->getSuffix()));

    int     i = 0;
    for(auto it = dataset->begin(); it != dataset->end(); it++)
        ret->insert(it.key(), Yest(i++));
    return ret;
}

void Cosinor::MesorCI(QSharedPointer<CCData<double>> &upper, QSharedPointer<CCData<double>> &lower)
{
    int i = 0;
    for(auto it = dataset->begin(); it != dataset->end(); it++) {
        upper->insert(it.key(), Yest(i) + getPack()->CI_M);
        lower->insert(it.key(), Yest(i++) - getPack()->CI_M);
    }
}


/*
 *  Set phi to the correct quadrant according to Signs of alpha and beta
 */
// Bingham et al. 1997
void Cosinor::correctPhi()
{
    getPack()->phi = std::atan( std::abs( gamma / beta ) );

    if(beta >= 0) {
        if(gamma >= 0)
            getPack()->phi *= -1;
        else
            getPack()->phi += -2 * M_PI;
    }
    else {
        if(gamma >= 0)
            getPack()->phi -= M_PI;
        else
            getPack()->phi = -1 * getPack()->phi - M_PI;
    }
}


void Cosinor::estimateCurve()
{
    Yest.resize(getPack()->N);
    for(int i = 0; i < getPack()->N; i++)
        //Yest(i) = M + mbeta * x(i) + mgamma * z(i);
        Yest(i) = getPack()->M + getPack()->A * std::cos(omega * t(i) * getPack()->phi);
}


/*
 *  Sum of Squares for Regression Analysis
 */
// Cornelissen 2014
void Cosinor::sumOfSquares()
{
    getPack()->MSS = (Yest - Y.mean()).square().sum();
    getPack()->RSS = (Y - Yest).square().sum();
    //TSS = MSS + RSS;
}


/*
 *  Calculation of Cosinor-Function. WIll be called by Constructor
 */
// Cornelissen 2014
void Cosinor::calc()
{
    Vector3d            d, u;
    ArrayXd             x, z;
    Matrix3d            S;
std::cout.precision(std::numeric_limits<double>::digits10);
    x = t.unaryExpr([&](double val){ return std::cos(Cosinor::omega * val); });
    z = t.unaryExpr([&](double val){ return std::sin(Cosinor::omega * val); });


    double                      x_sum = x.sum(),
                                z_sum = z.sum(),
                                xz_sum = x.cwiseProduct(z).sum();

    std::cout << x_sum << z_sum << xz_sum << std::endl;

    S <<    getPack()->N,          x_sum,                      z_sum,
            x_sum,      x.square().sum(),           xz_sum,
            z_sum,      xz_sum,                     z.square().sum();

    d <<    Y.sum(),
            Y.cwiseProduct(x).sum(),
            Y.cwiseProduct(z).sum();

    std::cout << S << d << std::endl;

    u = S.colPivHouseholderQr().solve(d);

    getPack()->M = u(0);
    beta = u(1);
    gamma = u(2);

    getPack()->A = std::sqrt(std::pow(beta, 2) + std::pow(gamma, 2));

    correctPhi();

    estimateCurve();

    sumOfSquares();

    getPack()->Sinv = S.inverse();
}

/*
 *  Helper-Functions for a Multitude of Confidence-interval Calculations etc.
 */
// According to Bingham et al. 1997
double Cosinor::c22() const
{
    return getPack()->Sinv(1, 1) * std::pow( std::cos( getPack()->phi ), 2 ) - 2 * getPack()->Sinv(1, 2) * std::cos( getPack()->phi ) * std::sin( getPack()->phi ) + getPack()->Sinv(2,2) * std::pow( std::sin( getPack()->phi ), 2 );
}


double Cosinor::c23() const
{
    return (getPack()->Sinv(1, 1) - getPack()->Sinv(2, 2)) * std::cos( getPack()->phi ) * std::sin( getPack()->phi ) + getPack()->Sinv(1, 2) * ( std::pow( std::cos( getPack()->phi ), 2 ) - std::pow( std::sin( getPack()->phi ), 2 ) );
}


double Cosinor::c33() const
{
    return getPack()->Sinv(1, 1) * std::pow( std::sin( getPack()->phi ), 2 ) + 2 * getPack()->Sinv(1, 2) * std::cos( getPack()->phi ) * std::sin( getPack()->phi ) + getPack()->Sinv(2, 2) * std::pow( std::cos( getPack()->phi ), 2 );
}


void Cosinor::mesorCI(const double &alpha)
{
    getPack()->CI_M = stats::qt(1-alpha/2, getPack()->N - 3) * sigma() * std::sqrt(getPack()->Sinv(0, 0));
}


void Cosinor::acroPhaseCI(const double &alpha)
{
    double          t = stats::qt( 1 - alpha / 2, getPack()->N - 3 );
    getPack()->CI_phi =
        std::atan(
            ( t * std::sqrt( c33() ) * sigma() ) /
            ( getPack()->A * std::sqrt( 1 - c22() * std::pow(t, 2) * std::pow(sigma(), 2) / std::pow( getPack()->A, 2 ) ) )
                );
}


void Cosinor::amplitudeCI(const double &alpha)
{
    getPack()->CI_A = sigma() * std::sqrt( c22() ) * stats::qt(1 - alpha / 2, getPack()->N - 3);
}


void Cosinor::zeroAmplitude(const double &alpha)
{
    getPack()->F = (getPack()->MSS/2) / (getPack()->RSS/(getPack()->N-3));
    getPack()->ZeroF = stats::qf<double, double>( 1 - alpha, 2, getPack()->N - 3);
}

/*
 *  Are Variances random?
 */
// Cornelissen 2014
void Cosinor::runsTest(const double &alpha)
{
    double                      med;
    quint64                     n1, n2;
    std::vector<double>         y(Yest.data(), Yest.data() + Yest.size());

    std::sort(y.begin(), y.end());

    if(Yest.size() % 2 != 0)
        med = y[y.size() / 2];
    else
        med = 0.5 * (y[y.size() / 2] + y[y.size() / 2 + 1]);

    for( n1 = 0; y[n1] < med; n1++);

    n2 = y.size() - n1;

    getPack()->R = ( runsTestRuns - ( ( 2 * n1 * n2 ) / y.size() + 1 ) ) / std::sqrt( ( 2 * n1 * n2 * ( 2 * n1 * n2 - y.size() ) ) / std::pow( y.size(), 2) * ( n1 + n2 - 1 ));
    getPack()->RTest = stats::qnorm( 1 - alpha / 2, 0.0, 1.0);
}

/*
 *  Test Normaldistribution for Variances
 */
// Cornelissen 2014
CCDoubleDataPtr Cosinor::rankitPlot() const
{
    QVector<double>          e(getPack()->N), rankit(getPack()->N);

    for(int i = 0; i < getPack()->N; i++) {
        e[i] = Y(i) - Yest(i);
        rankit[i] = stats::qnorm((i + 0.5) / getPack()->N);
    }

    std::sort(e.begin(), e.end());

    CCDoubleDataPtr     ret(new CCData<double>("Rankit"));

    for(int i = 0; i < getPack()->N; i++)
        ret->insert(rankit[i], e[i]);

    return ret;
}

/*
 *  Are Variances about Equal?
 */
// Cornelissen 2014
CCDoubleDataPtr Cosinor::variancePlot() const
{
    CCDoubleDataPtr        ret(new CCData<double>("Variances"));

    for(int i = 0; i < getPack()->N; i++)
        ret->insert(Y(i), Y(i) - Yest(i));

    return ret;
}

/*
 *  Is Least-Squares Regression the right kind of Analysis for this kind of Data?
 */
// Cornelissen 2014
void Cosinor::modelAdequacy(const QVector<CCDataSetPtr> &compare, const double &alpha) const
{
    double       SSPE = getPack()->MSS + getPack()->RSS;
    int             p = 1, k = compare.size();      // Single-Component Cosinor

    for(int i = 0; i < k; i++) {
       /* if(!compare[i]->isType(AlgorithmType::Cosinor))
            continue;*/
        CosinorData        *stats = static_cast<CosinorData*>(compare[i]->getStatistics());
        SSPE += stats->MSS + stats->RSS;
    }

    /*getPack()->adequacy = ((getPack()->RSS - SSPE) / (k - 1 - 2 * p)) / (SSPE / (getPack()->N - k));
    getPack()->adequacyTest = stats::qf<double, qint64>( 1 - alpha, k - 1 - 2 * p, getPack()->N - k );*/
}
