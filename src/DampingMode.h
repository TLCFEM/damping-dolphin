/*******************************************************************************
 * Copyright (C) 2022-2026 Theodore Chang
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

#ifndef DAMPINGMODE_H
#define DAMPINGMODE_H

#include "damping-dolphin.h"

class DampingMode {
protected:
    const MT type;
    const double omega_p, zeta_p;
    std::vector<double> p;

public:
    DampingMode(double, double, std::vector<double>&&, MT);
    virtual ~DampingMode() = default;

    virtual double operator()(double) const = 0;

    virtual void tidyUp();

    virtual QString str() const = 0;
    virtual QString command() const = 0;
};

class DampingModeT0 : public DampingMode {
public:
    DampingModeT0(double, double, std::vector<double>&&);

    double operator()(double) const override;

    QString str() const override;
    QString command() const override;
};

class DampingModeT1 : public DampingMode {
public:
    DampingModeT1(double, double, std::vector<double>&&);

    double operator()(double) const override;

    void tidyUp() override;

    QString str() const override;
    QString command() const override;
};

class DampingModeT2 : public DampingMode {
public:
    DampingModeT2(double, double, std::vector<double>&&);

    double operator()(double) const override;

    void tidyUp() override;

    QString str() const override;
    QString command() const override;
};

class DampingModeT3 : public DampingMode {
public:
    DampingModeT3(double, double, std::vector<double>&&);

    double operator()(double) const override;

    QString str() const override;
    QString command() const override;
};

class DampingModeT4 : public DampingMode {
public:
    DampingModeT4(double, double, std::vector<double>&&);

    double operator()(double) const override;

    void tidyUp() override;

    QString str() const override;
    QString command() const override;
};

#endif // DAMPINGMODE_H
