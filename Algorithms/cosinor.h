#ifndef COSINOR_H
#define COSINOR_H

#include "../dataset.h"

#include <../include/Eigen/Core>
#include <../include/Eigen/LU>
#include <../include/Eigen/QR>

#include <../include/StatsLib/include/stats.hpp>


// Typedefs

/*#define EArrayXld       Eigen::Array<double, Eigen::Dynamic, 1>
#define EMatrix3ld      Eigen::Matrix<double, 3, 3>
#define EVector3ld      Eigen::Matrix<double, 3, 1>*/

/**
 * @brief The CosinorDataType enum is used for Saving calculated Cosinor-Data into the Database
 */
enum class CosinorDataType : qint32 {
    Sc11 = 0, Sc12, Sc13, Sc21, Sc22, Sc23, Sc31, Sc32, Sc33,
    Mesor,
    Amplitude,
    Acrophase,
    RSS,
    MSS,
    Period,
    Size
};


/**
 * @brief The CosinorData struct
 */
struct CosinorData : public AlgorithmData
{
    double              M, A, phi, RSS, MSS, tau;
    double              CI_M, CI_A, CI_phi, F, ZeroF, R, RTest;
    qint64              N;
    Eigen::Matrix3d     Sinv;

    CosinorData() : AlgorithmData(), M(0), A(0), phi(0), RSS(0), MSS(0), tau(0), N(0) {}
    CosinorData(const CosinorData &other) : AlgorithmData(other), M(other.M), A(other.A), phi(other.phi), RSS(other.RSS),
        MSS(other.MSS), tau(other.tau), N(other.N), Sinv(other.Sinv) {}
    ~CosinorData() override {}

    void load(const QString &s) override {
        QStringList     l = s.split(";");

        for(int i=0;i<3;i++)
            for(int j=0;j<3;j++)
                Sinv(i, j) = l[i * 3 + j].toDouble();
                //ret->set("Sinv" + QString::number(i * 3 + j), Sinv(i, j));

        M = l[static_cast<int>(CosinorDataType::Mesor)].toDouble();
        A = l[static_cast<int>(CosinorDataType::Amplitude)].toDouble();
        phi = l[static_cast<int>(CosinorDataType::Acrophase)].toDouble();
        RSS = l[static_cast<int>(CosinorDataType::RSS)].toDouble();
        MSS = l[static_cast<int>(CosinorDataType::MSS)].toDouble();
        tau = l[static_cast<int>(CosinorDataType::Period)].toDouble();
        N = l[static_cast<int>(CosinorDataType::Size)].toLongLong();
    }


    QString save() const override {
        QStringList         l;

        for(int i=0;i<3;i++)
            for(int j=0;j<3;j++)
                l << QString::number(Sinv(i, j));
        l << QString::number(M);
        l << QString::number(A);
        l << QString::number(phi);
        l << QString::number(RSS);
        l << QString::number(MSS);
        l << QString::number(tau);
        l << QString::number(N);

        return l.join(";");
    }


    QStringList toStringList() const override {
        return {
                    "Mesor:", QString::number(M),
                    "-- CI:", "[" + QString::number(M - CI_M) + "; " + QString::number(M + CI_M) + "]",
                    "Amplitude:", QString::number(A),
                    "-- CI:", "[" + QString::number(A - CI_A) + "; " + QString::number(A + CI_A) + "]",
                    "Acrophase:", QString::number(phi),
                    "-- CI:", "[" + QString::number(phi - CI_phi) + "; " + QString::number(phi + CI_phi) + "]",
                    "Zero Amplitude:", QString::number(F) + " > " + QString::number(ZeroF),
                    "Runs Test:", QString::number(R) + " <> " + QString::number(RTest)
        };
    }
};


/**
 * @brief The Cosinor class
 */
class Cosinor : public AlgorithmBase<CCDataPtr>
{
public:

    // Public Static Members

    static int                          runsTestRuns;

    // Public Methods

    Cosinor(const CCDataSetPtr &_dataset, double _timePeriod);
    Cosinor(CosinorData *dt, const CCDataSetPtr &_cosinor, const CCDataSetPtr &_parent);

    void                    recalc(const double &alpha);
    CCDataPtr               getData() const override;
    inline CosinorData      *getPack() const { return reinterpret_cast<CosinorData*>(data); }

    void                    MesorCI(CCDoubleDataPtr &upper, CCDoubleDataPtr &lower);
    CCDoubleDataPtr         rankitPlot() const;
    CCDoubleDataPtr         variancePlot() const;

private:

    // Private Static Members

    static constexpr double             omega = 2 * M_PI;

    // Private Methods

    void                    correctPhi();
    void                    estimateCurve();
    void                    sumOfSquares();
    void                    calc();

    inline double           sigma() const { return std::sqrt( getPack()->RSS / (getPack()->N - 3)); } // Cornelissen 2014
    double                  c22() const;
    double                  c23() const;
    double                  c33() const;
    void                    mesorCI(const double &alpha);
    void                    acroPhaseCI(const double &alpha);
    void                    amplitudeCI(const double &alpha);
    void                    zeroAmplitude(const double &alpha);
    void                    runsTest(const double &alpha);
    void                    init();
    void                    modelAdequacy(const QVector<QSharedPointer<CCDataSet> > &compare, const double &alpha) const;

    // Private Members

    double                  beta, gamma;
    Eigen::ArrayXd          t, Y, Yest;
    CCDataSetPtr            dataset;


};

#endif // COSINOR_H
