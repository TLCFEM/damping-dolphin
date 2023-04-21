/*******************************************************************************
 * Copyright (C) 2022-2023 Theodore Chang
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

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
