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

#include "ThreeWiseMen.h"
#include "parallel_for.hpp"

double ThreeWiseMen::compute_response(const double x, const vec& p) {
    const auto& w = p(0);
    const auto& z = p(1);
    const auto& g = p(2);

    const auto omega_r = x / w;
    const auto logw = log(omega_r);
    const auto coshlog = cosh(logw);

    return z * (1. + g) * coshlog / (coshlog * coshlog + g);
}

vec ThreeWiseMen::compute_gradient(const double x, const vec& p) {
    vec out(num_para + 1);

    const auto& w = p(0);
    const auto& z = p(1);
    const auto& g = p(2);

    const auto omega_r = x / w;
    const auto logw = log(omega_r);
    const auto coshlog = cosh(logw);
    const auto factor = coshlog * coshlog + g;

    out(2) = (1. + g) * coshlog / factor;
    out(0) = z * out(2);
    out(1) = z * (1. + g) * (coshlog * coshlog - g) * sinh(logw) / w * pow(factor, -2.);
    out(3) = coshlog * (coshlog * coshlog - 1.) * z * pow(factor, -2.);

    return out;
}

vec ThreeWiseMen::s(const vec& p) const {
    vec sp(num_para);

    sp(0) = pow(10., min_omega + range_omega / (1. + exp(-p(0))));
    sp(1) = max_zeta / (1. + exp(-p(1)));
    sp(2) = p(2) * p(2) - .98;

    return sp;
}

vec ThreeWiseMen::ds(const vec& p) const {
    vec dsp(num_para);

    auto expp = exp(-abs(p(0)));
    dsp(0) = log(10.) * expp * pow(1. + expp, -2.) * pow(10., min_omega + range_omega / (1. + exp(-p(0)))) * range_omega;

    expp = exp(-abs(p(1)));
    dsp(1) = max_zeta * expp * pow(1. + expp, -2.);

    dsp(2) = 2. * p(2);

    return dsp;
}

unsigned ThreeWiseMen::getSize() const {
    return num_para;
}

double ThreeWiseMen::EvaluateWithGradient(const mat& x, mat& g) {
    mat dg(num_para * num_modes, sampling.n_cols, fill::none);

    for(auto J = 0u; J < num_modes; ++J) {
        const vec p(&x(num_para * J), num_para);
        const auto sp = s(p);
        const auto dsp = ds(p);
        dd::parallel_for(0llu, sampling.n_cols, [&](const uword I) {
            const auto grad = compute_gradient(sampling(0, I), sp);
            response(J, I) = grad(0);
            vec(&dg(num_para * J, I), num_para, false, true) = grad.tail(num_para) % dsp;
        });
    }

    const rowvec fi = sum(response, 0) - sampling.row(1);

    dd::parallel_for(0llu, sampling.n_cols, [&](const uword I) { dg.col(I) *= 2. * fi(I); });

    g = sum(dg, 1);

    return accu(pow(fi, 2.));
}

QStringList ThreeWiseMen::getTypeList(const mat& result) const {
    QStringList list;

    for(auto I = 0llu; I < result.n_rows; ++I)
        list.append("Type 3 --- " + QString::number(result(I, 0)) + " " + QString::number(result(I, 1)) + " " + QString::number(result(I, 2)));

    return list;
}
