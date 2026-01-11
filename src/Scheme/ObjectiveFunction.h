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

#ifndef OBJECTIVEFUNCTION_H
#define OBJECTIVEFUNCTION_H

#include "../damping-dolphin.h"

class ObjectiveFunction {
protected:
    const unsigned num_modes;

    mat sampling, response;

    double min_omega = 0., max_omega = 0., min_zeta = 0., max_zeta = 0.;
    double range_omega = 0.;

    uvec base;

    double weight = 0.;

    int max_order = 10;

public:
    [[nodiscard]] virtual vec s(const vec&) const;
    [[nodiscard]] virtual vec ds(const vec&) const;

    explicit ObjectiveFunction(unsigned);
    virtual ~ObjectiveFunction() = default;

    void initializeSampling(mat&&);

    void setWeight(double);
    void setMaxOrder(int);

    [[nodiscard]] virtual unsigned getSize() const = 0;
    [[nodiscard]] unsigned getNumberModes() const;

    [[nodiscard]] virtual size_t NumConstraints() const;

    virtual double Evaluate(const mat&);
    virtual void Gradient(const mat&, mat&);

    virtual double EvaluateConstraint(size_t, const mat&);
    virtual void GradientConstraint(size_t, const mat&, mat&);

    virtual double EvaluateWithGradient(const mat&, mat&) = 0;

    [[nodiscard]] virtual QStringList getTypeList(const mat&) const = 0;
};

#endif // OBJECTIVEFUNCTION_H
