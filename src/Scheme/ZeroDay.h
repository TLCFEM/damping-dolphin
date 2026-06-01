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

#ifndef ZERODAY_H
#define ZERODAY_H

#include "ObjectiveFunction.h"
#include "parallel_for.hpp"

template<typename ET> class ZeroDay : public ObjectiveFunction<ET> {
    static constexpr unsigned num_para = 2;

public:
    static ET compute_response(const ET x, const Col<ET>& p) {
        const auto& w = p(0);
        const auto& z = p(1);

        const auto wr = x / w;

        return z * ET(2) * wr / (ET(1) + wr * wr);
    }
    static Col<ET> compute_gradient(const ET x, const Col<ET>& p) {
        Col<ET> out(num_para + 1);

        const auto& w = p(0);
        const auto& z = p(1);

        const auto wr = x / w;
        const auto factor = wr * wr + ET(1);

        out(2) = ET(2) * wr / factor;
        out(0) = out(2) * z;
        out(1) = out(0) / w * (wr * wr - ET(1)) / factor;

        return out;
    }

    [[nodiscard]] Col<ET> s(const Col<ET>& p) const override {
        Col<ET> sp(num_para);

        sp(0) = pow(ET(10), this->min_omega + this->range_omega / (ET(1) + exp(-p(0))));
        sp(1) = this->max_zeta / (ET(1) + exp(-p(1)));

        return sp;
    }
    [[nodiscard]] Col<ET> ds(const Col<ET>& p) const override {
        const auto expw = exp(-abs(p(0)));
        const auto expz = exp(-abs(p(1)));

        Col<ET> dsp(num_para);

        dsp(0) = log(ET(10)) * expw * pow(ET(1) + expw, -ET(2)) * pow(ET(10), this->min_omega + this->range_omega / (ET(1) + exp(-p(0)))) * this->range_omega;
        dsp(1) = this->max_zeta * expz * pow(ET(1) + expz, -ET(2));

        return dsp;
    }

    using ObjectiveFunction<ET>::ObjectiveFunction;

    [[nodiscard]] unsigned getSize() const override { return num_para; }

    ET EvaluateWithGradient(const Mat<ET>& x, Mat<ET>& g) override {
        Mat<ET> dg(num_para * this->num_modes, this->sampling.n_cols, fill::none);

        for(auto J = 0u; J < this->num_modes; ++J) {
            const Col<ET> p(&x(num_para * J), num_para);
            const auto sp = s(p);
            const auto dsp = ds(p);
            dd::parallel_for(0llu, this->sampling.n_cols, [&](const uword I) {
                const auto grad = compute_gradient(this->sampling(0, I), sp);
                this->response(J, I) = grad(0);
                Col<ET>(&dg(num_para * J, I), num_para, false, true) = grad.tail(num_para) % dsp;
            });
        }

        const Row<ET> fi = sum(this->response, 0) - this->sampling.row(1);

        dd::parallel_for(0llu, this->sampling.n_cols, [&](const uword I) { dg.col(I) *= ET(2) * fi(I); });

        g = sum(dg, 1);

        return accu(pow(fi, ET(2)));
    }

    [[nodiscard]] QStringList getTypeList(const Mat<ET>& result) const override {
        QStringList list;

        for(auto I = 0llu; I < result.n_rows; ++I)
            list.append("Type 0 --- " + QString::number(result(I, 0)) + " " + QString::number(result(I, 1)));

        return list;
    }
};

#endif // ZERODAY_H
