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

#ifndef ZERODAY_H
#define ZERODAY_H

#include "ObjectiveFunction.h"

class ZeroDay : public ObjectiveFunction {
    static constexpr unsigned num_para = 2;

public:
    static double compute_response(double, const vec&);
    static vec compute_gradient(double, const vec&);

    [[nodiscard]] vec s(const vec&) const override;
    [[nodiscard]] vec ds(const vec&) const override;

    using ObjectiveFunction::ObjectiveFunction;

    [[nodiscard]] unsigned getSize() const override;

    double EvaluateWithGradient(const mat&, mat&) override;

    [[nodiscard]] QStringList getTypeList(const mat&) const override;
};

#endif // ZERODAY_H
