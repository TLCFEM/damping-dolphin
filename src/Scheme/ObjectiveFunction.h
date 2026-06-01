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

template<typename ET> class ObjectiveFunction {
protected:
    const unsigned num_modes;

    Mat<ET> sampling, response;

    ET min_omega{0}, max_omega{0}, min_zeta{0}, max_zeta{0};
    ET range_omega{0};

    uvec base;

    ET weight{0};

    int max_order = 10;

public:
    [[nodiscard]] virtual Col<ET> s(const Col<ET>& p) const { return p; }
    [[nodiscard]] virtual Col<ET> ds(const Col<ET>& p) const { return ones(size(p)); }

    explicit ObjectiveFunction(const unsigned S)
        : num_modes(S) {}
    virtual ~ObjectiveFunction() = default;

    void initializeSampling(Mat<ET>&& T) {
        sampling = std::move(T);
        response.set_size(num_modes, sampling.n_cols);
        min_omega = log10(min(sampling.row(0))) - .1;
        max_omega = log10(max(sampling.row(0))) + .1;
        min_zeta = min(sampling.row(1));
        max_zeta = max(sampling.row(1));
        range_omega = max_omega - min_omega;

        base = linspace<uvec>(0, num_modes - 1, num_modes);
    }

    void setWeight(const ET W) { weight = W; }
    void setMaxOrder(const int M) { max_order = M; }

    [[nodiscard]] virtual unsigned getSize() const = 0;
    [[nodiscard]] unsigned getNumberModes() const { return num_modes; }

    [[nodiscard]] virtual size_t NumConstraints() const { return 0; }

    virtual ET Evaluate(const Mat<ET>& x) {
        Mat<ET> g;

        return EvaluateWithGradient(x, g);
    }
    virtual void Gradient(const Mat<ET>& x, Mat<ET>& g) { EvaluateWithGradient(x, g); }

    virtual ET EvaluateConstraint(const size_t, const Mat<ET>&) { return ET(0); }
    virtual void GradientConstraint(const size_t, const Mat<ET>& x, Mat<ET>& g) { g.zeros(size(x)); }

    virtual ET EvaluateWithGradient(const Mat<ET>&, Mat<ET>&) = 0;

    [[nodiscard]] virtual QStringList getTypeList(const Mat<ET>&) const = 0;
};

#endif // OBJECTIVEFUNCTION_H
