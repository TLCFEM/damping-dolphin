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

#include "ZeroDay.h"
#include "parallel_for.hpp"

double ZeroDay::compute_response(const double x, const vec& p) {
    const auto& w = p(0);
    const auto& z = p(1);

    const auto wr = x / w;

    return z * 2. * wr / (1. + wr * wr);
}

vec ZeroDay::compute_gradient(const double x, const vec& p) {
    vec out(num_para + 1);

    const auto& w = p(0);
    const auto& z = p(1);

    const auto wr = x / w;
    const auto factor = wr * wr + 1.;

    out(2) = 2. * wr / factor;
    out(0) = out(2) * z;
    out(1) = out(0) / w * (wr * wr - 1.) / factor;

    return out;
}

vec ZeroDay::s(const vec& p) const {
    vec sp(num_para);

    sp(0) = pow(10., min_omega + range_omega / (1. + exp(-p(0))));
    sp(1) = max_zeta / (1. + exp(-p(1)));

    return sp;
}

vec ZeroDay::ds(const vec& p) const {
    const auto expw = exp(-abs(p(0)));
    const auto expz = exp(-abs(p(1)));

    vec dsp(num_para);

    dsp(0) = log(10.) * expw * pow(1. + expw, -2.) * pow(10., min_omega + range_omega / (1. + exp(-p(0)))) * range_omega;
    dsp(1) = max_zeta * expz * pow(1. + expz, -2.);

    return dsp;
}

unsigned ZeroDay::getSize() const { return num_para; }

double ZeroDay::EvaluateWithGradient(const mat& x, mat& g) {
    mat dg(num_para * num_modes, sampling.n_cols, fill::none);

    for(auto J = 0u; J < num_modes; ++J) {
        const vec p(&x(num_para * J), num_para);
        const auto sp = s(p);
        const auto dsp = ds(p);
        dd::parallel_for(0llu, sampling.n_cols, [&](const uword I) {
            const auto grad = compute_gradient(sampling(0, I), sp);
            response(J, I) = grad(0);
            vec gi(&dg(num_para * J, I), num_para, false, true);
            gi = grad.tail(num_para) % dsp;
        });
    }

    const rowvec fi = sum(response, 0) - sampling.row(1);

    dd::parallel_for(0llu, sampling.n_cols, [&](const uword I) { dg.col(I) *= 2. * fi(I); });

    g = sum(dg, 1);

    return accu(pow(fi, 2.));
}

QStringList ZeroDay::getTypeList(const mat& result) const {
    QStringList list;

    for(auto I = 0llu; I < result.n_rows; ++I)
        list.append("Type 0 --- " + QString::number(result(I, 0)) + " " + QString::number(result(I, 1)));

    return list;
}
