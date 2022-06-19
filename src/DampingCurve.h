#ifndef DAMPINGCURVE_H
#define DAMPINGCURVE_H

#include "damping-dolphin.h"

class DampingMode;

class ControlPoint {
    QVector<double> omega;
    QVector<double> zeta;

public:
    void addPoint(double, double);
    void removePoint(int = -1);

    const QVector<double>& getFrequencyVector();
    const QVector<double>& getDampingRatioVector();

    double minFrequency();
    double maxFrequency();
    double minDampingRatio();
    double maxDampingRatio();

    mat getSampling();

    int count();
};

class DampingCurve {
    QVector<std::shared_ptr<DampingMode>> damping_modes;
    QVector<QVector<double>> zeta;
    QVector<double> zeta_sum;
    QVector<double> omega;

    void initializeVector(size_t);
    void computeCurve();

public:
    void addMode(std::unique_ptr<DampingMode>&&);
    void removeMode(int = -1);

    void updateLinearDampingCurve(double, double, size_t);
    void updateLogarithmicDampingCurve(double, double, size_t);

    double query(double);

    const QVector<double>& getFrequencyVector();
    const QVector<double>& getDampingRatioVector(int = -1);

    QStringList getTypeInfo();
    QStringList getCommand();
    int count();

    double minFrequency();
    double maxFrequency();
    double minDampingRatio();
    double maxDampingRatio();

    void tidyUp();
};

#endif // DAMPINGCURVE_H
