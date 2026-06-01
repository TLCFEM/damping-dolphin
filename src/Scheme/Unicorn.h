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

#ifndef UNICORN_H
#define UNICORN_H

#include "ObjectiveFunction.h"
#include "parallel_for.hpp"

template<typename ET> class Unicorn : public ObjectiveFunction<ET> {
    static constexpr unsigned num_para = 3;

    static Mat<ET> decimal(const Mat<ET>& n) {
        return n - arma::round(n);
    }

public:
    static ET compute_response(const ET x, const Col<ET>& p) {
        const auto& w = p(0);
        const auto& z = p(1);
        const auto& n = p(2);

        const auto omega_r = x / w;
        const auto logw = log(omega_r);
        const auto coshlog = cosh(logw);

        return z * pow(coshlog, -ET(2) * n - ET(1));
    }
    static Col<ET> compute_gradient(const ET x, const Col<ET>& p) {
        Col<ET> out(num_para + 1);

        const auto& w = p(0);
        const auto& z = p(1);
        const auto& n = p(2);

        const auto omega_r = x / w;
        const auto logw = log(omega_r);
        const auto coshlog = cosh(logw);

        out(2) = pow(coshlog, -ET(2) * n - ET(1));
        out(0) = z * out(2);
        out(1) = (ET(2) * n + ET(1)) * out(0) / coshlog * sinh(logw) / w;
        out(3) = -ET(2) * out(2) * z * log(coshlog);

        return out;
    }

    [[nodiscard]] Col<ET> s(const Col<ET>& p) const override {
        Col<ET> sp(num_para);

        sp(0) = pow(ET(10), this->min_omega + this->range_omega / (ET(1) + exp(-p(0))));
        sp(1) = this->max_zeta / (ET(1) + exp(-p(1)));
        sp(2) = this->max_order / (ET(1) + exp(-p(2)));

        return sp;
    }
    [[nodiscard]] Col<ET> ds(const Col<ET>& p) const override {
        Col<ET> dsp(num_para);

        auto expp = exp(-abs(p(0)));
        dsp(0) = log(ET(10)) * expp * pow(ET(1) + expp, -ET(2)) * pow(ET(10), this->min_omega + this->range_omega / (ET(1) + exp(-p(0)))) * this->range_omega;

        expp = exp(-abs(p(1)));
        dsp(1) = this->max_zeta * expp * pow(ET(1) + expp, -ET(2));

        expp = exp(-abs(p(2)));
        dsp(2) = this->max_order * expp * pow(ET(1) + expp, -ET(2));

        return dsp;
    }

    using ObjectiveFunction<ET>::ObjectiveFunction;

    [[nodiscard]] unsigned getSize() const override { return num_para; }

    ET EvaluateWithGradient(const Mat<ET>& x, Mat<ET>& g) override {
        Mat<ET> dg(num_para * this->num_modes, this->sampling.n_cols, fill::none);
        Col<ET> n(this->num_modes, fill::none);
        Col<ET> dn(this->num_modes, fill::none);

        for(auto J = 0u; J < this->num_modes; ++J) {
            const Col<ET> p(&x(num_para * J), num_para);
            const auto sp = s(p);
            const auto dsp = ds(p);
            n(J) = sp(2);
            dn(J) = dsp(2);
            dd::parallel_for(0llu, this->sampling.n_cols, [&](const uword I) {
                const auto grad = compute_gradient(this->sampling(0, I), sp);
                this->response(J, I) = grad(0);
                Col<ET>(&dg(num_para * J, I), num_para, false, true) = grad.tail(num_para) % dsp;
            });
        }

        const Row<ET> fi = sum(this->response, 0) - this->sampling.row(1);

        dd::parallel_for(0llu, this->sampling.n_cols, [&](const uword I) { dg.col(I) *= ET(2) * fi(I); });

        g = sum(dg, 1);

        const Col<ET> floor_diff = decimal(n);

        g(num_para * this->base + 2) += ET(2) * this->weight * floor_diff % dn;

        return accu(pow(fi, ET(2))) + this->weight * accu(pow(floor_diff, ET(2)));
    }

    [[nodiscard]] size_t NumConstraints() const override { return this->num_modes; }

    ET EvaluateConstraint(const size_t i, const Mat<ET>& x) override {
        const Col<ET> p(&x(num_para * i), num_para);

        const Col<ET> floor_diff = decimal(Mat<ET>{s(p)(2)});

        return this->weight * floor_diff(0) * floor_diff(0);
    }
    void GradientConstraint(const size_t i, const Mat<ET>& x, Mat<ET>& g) override {
        const Col<ET> p(&x(num_para * i), num_para);

        const Col<ET> floor_diff = decimal(Mat<ET>{s(p)(2)});

        g = zeros<Mat<ET>>(size(x));
        g(num_para * i + 2) = ET(2) * this->weight * floor_diff(0) * ds(p)(2);
    }

    [[nodiscard]] QStringList getTypeList(const Mat<ET>& result) const override {
        QStringList list;

        for(auto I = 0llu; I < result.n_rows; ++I)
            list.append("Type 1 --- " + QString::number(result(I, 0)) + " " + QString::number(result(I, 1)) + " " + QString::number(result(I, 2)));

        return list;
    }
};

#endif // UNICORN_H
