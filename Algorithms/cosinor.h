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

struct CosinorData : public AlgorithmData
{
    double              M, A, phi, RSS, MSS, tau;
    double              CI_M, CI_A, CI_phi, F, ZeroF;
    qint64              N;
    Eigen::Matrix3d     Sinv;

    CosinorData() :
        AlgorithmData(), M(0), A(0), phi(0), RSS(0), MSS(0), tau(0), N(0) {}
    CosinorData(const CosinorData &other) :
        AlgorithmData(other), M(other.M), A(other.A), phi(other.phi), RSS(other.RSS),
        MSS(other.MSS), tau(other.tau), N(other.N), Sinv(other.Sinv) {}
    ~CosinorData() override {}

    QStringList toString() const override {
        return {
                    "Mesor:", QString::number(M),
                    "-- CI:", "[" + QString::number(M - CI_M) + "; " + QString::number(M + CI_M) + "]",
                    "Amplitude:", QString::number(A),
                    "-- CI:", "[" + QString::number(A - CI_A) + "; " + QString::number(A + CI_A) + "]",
                    "Acrophase:", QString::number(phi),
                    "-- CI:", "[" + QString::number(phi - CI_phi) + "; " + QString::number(phi + CI_phi) + "]",
                    "Zero Amplitude:", QString::number(F) + " > " + QString::number(ZeroF)
        };
    }
};


class Cosinor : public AlgorithmBase<CCDataPtr>
{
public:
    explicit Cosinor(CCDataSetPtr &_dataset, double _timePeriod);
    explicit Cosinor(CCDataSetPtr &_cosinor, QSharedPointer<CCDataSet> &_parent, CosinorData *dt);

    void recalc(const double &alpha);
    CCDataPtr getData() const override;
    inline CosinorData *getPack() const { return reinterpret_cast<CosinorData*>(data); }

private:

    static constexpr double             omega = 2 * M_PI;

    // Private Methods

    void                                correctPhi();
    void                                estimateCurve();
    void                                sumOfSquares();
    void                                calc();

    inline double               sigma() const { return std::sqrt( getPack()->RSS / (getPack()->N - 3)); } // Cornelissen 2014
    double                      c22() const;
    double                      c23() const;
    double                      c33() const;
    void                        mesorCI(const double &alpha);
    void                        acroPhaseCI(const double &alpha);
    void                        amplitudeCI(const double &alpha);
    void                        zeroAmplitude(const double &alpha);

    // Private Members

    double                      beta, gamma;
    //double              M, A, phi, beta, gamma, RSS, MSS;
    //qint64              N, tau;
    //Eigen::Matrix3d     Sinv;
    Eigen::ArrayXd      t, Y, Yest;
    CCDataSetPtr        dataset;

    void init();
};

#endif // COSINOR_H
