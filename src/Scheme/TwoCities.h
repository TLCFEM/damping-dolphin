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

#ifndef TWOCITIES_H
#define TWOCITIES_H

#include "ObjectiveFunction.h"
#include "parallel_for.hpp"

template<typename ET> class TwoCities : public ObjectiveFunction<ET> {
    static constexpr unsigned num_para = 4;

    static Mat<ET> decimal(const Mat<ET>& n) {
        return n - arma::round(n);
    }

public:
    static ET compute_response(const ET x, const Col<ET>& p) {
        const auto& w = p(0);
        const auto& z = p(1);
        const auto& nr = p(2);
        const auto& nl = p(3);

        const auto ra = ET(2) * nl + ET(1);
        const auto rb = ET(2) * nr + ET(1);
        const auto r = ra / rb;
        const auto xr = x / w;

        return z * (ET(1) + r) * pow(xr, ET(2) * nl + ET(1)) / (ET(1) + r * pow(xr, ET(2) * (ET(1) + nr + nl)));
    }
    static Col<ET> compute_gradient(const ET x, const Col<ET>& p) {
        Col<ET> out(num_para + 1);

        const auto& w = p(0);
        const auto& z = p(1);
        const auto& nr = p(2);
        const auto& nl = p(3);

        const auto xr = x / w;
        const auto nps = ET(1) + nr + nl;
        const auto ra = ET(2) * nl + ET(1);
        const auto rb = ET(2) * nr + ET(1);
        const auto r = ra / rb;

        const auto fa = (ET(1) + r) * pow(xr, ET(2) * nl + ET(1));
        const auto fb = ET(1) + r * pow(xr, ET(2) * nps);

        const auto fc = pow(xr, ra);
        const auto fd = pow(nr + ET(.5), ET(2));
        const auto fe = pow(xr, ET(2) * nps);

        const auto aw = -ET(2) * fc * ra * nps / (w * rb);
        const auto anr = -fc * (ET(1) * nl + ET(.5)) / fd;
        const auto anl = fc * (ET(4) * nps * log(xr) + ET(2)) / rb;

        const auto bw = -ET(2) * fe * ra * nps / (w * rb);
        const auto bnr = fe * ra * (ET(2) * fd * log(xr) - nr - ET(.5)) / (fd * rb);
        const auto bnl = ET(2) * fe * (ra * log(xr) + ET(1)) / rb;

        out(2) = fa / fb;
        out(0) = z * out(2);
        out(1) = z / fb * (aw - out(2) * bw);
        out(3) = z / fb * (anr - out(2) * bnr);
        out(4) = z / fb * (anl - out(2) * bnl);

        return out;
    }

    [[nodiscard]] Col<ET> s(const Col<ET>& p) const override {
        Col<ET> sp(num_para);

        sp(0) = pow(ET(10), this->min_omega + this->range_omega / (ET(1) + exp(-p(0))));
        sp(1) = this->max_zeta / (ET(1) + exp(-p(1)));
        sp(2) = this->max_order / (ET(1) + exp(-p(2)));
        sp(3) = this->max_order / (ET(1) + exp(-p(3)));

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

        expp = exp(-abs(p(3)));
        dsp(3) = this->max_order * expp * pow(ET(1) + expp, -ET(2));

        return dsp;
    }

    using ObjectiveFunction<ET>::ObjectiveFunction;

    [[nodiscard]] unsigned getSize() const override { return num_para; }

    ET EvaluateWithGradient(const Mat<ET>& x, Mat<ET>& g) override {
        Mat<ET> dg(num_para * this->num_modes, this->sampling.n_cols, fill::none);
        Mat<ET> n(2, this->num_modes, fill::none);
        Mat<ET> dn(2, this->num_modes, fill::none);

        for(auto J = 0u; J < this->num_modes; ++J) {
            const Col<ET> p(&x(num_para * J), num_para);
            const auto sp = s(p);
            const auto dsp = ds(p);
            n.col(J) = sp.tail(2);
            dn.col(J) = dsp.tail(2);
            dd::parallel_for(0llu, this->sampling.n_cols, [&](const uword I) {
                const auto grad = compute_gradient(this->sampling(0, I), sp);
                this->response(J, I) = grad(0);
                Col<ET>(&dg(num_para * J, I), num_para, false, true) = grad.tail(num_para) % dsp;
            });
        }

        const Row<ET> fi = sum(this->response, 0) - this->sampling.row(1);

        dd::parallel_for(0llu, this->sampling.n_cols, [&](const uword I) { dg.col(I) *= ET(2) * fi(I); });

        g = sum(dg, 1);

        const Mat<ET> floor_diff = decimal(n).t();

        g(num_para * this->base + 2) += ET(2) * this->weight * floor_diff.col(0) % dn.row(0).t();
        g(num_para * this->base + 3) += ET(2) * this->weight * floor_diff.col(1) % dn.row(1).t();

        return accu(pow(fi, ET(2))) + this->weight * accu(pow(floor_diff, ET(2)));
    }

    [[nodiscard]] size_t NumConstraints() const override { return 2 * this->num_modes; }

    ET EvaluateConstraint(const size_t i, const Mat<ET>& x) override {
        const Col<ET> p(&x(num_para * (i / 2)), num_para);

        const Col<ET> floor_diff = decimal(Mat<ET>{s(p)(i % 2 + 2)});

        return this->weight * floor_diff(0) * floor_diff(0);
    }
    void GradientConstraint(const size_t i, const Mat<ET>& x, Mat<ET>& g) override {
        const auto i_mode = i / 2;
        const auto i_shift = i % 2 + 2;

        const Col<ET> p(&x(num_para * i_mode), num_para);

        const Col<ET> floor_diff = decimal(Mat<ET>{s(p)(i_shift)});

        g = zeros(size(x));
        g(num_para * i_mode + i_shift) = ET(2) * this->weight * floor_diff(0) * ds(p)(i_shift);
    }

    [[nodiscard]] QStringList getTypeList(const Mat<ET>& result) const override {
        QStringList list;

        for(auto I = 0llu; I < result.n_rows; ++I)
            list.append("Type 2 --- " + QString::number(result(I, 0)) + " " + QString::number(result(I, 1)) + " " + QString::number(result(I, 2)) + " " + QString::number(result(I, 3)));

        return list;
    }
};

#endif // TWOCITIES_H
