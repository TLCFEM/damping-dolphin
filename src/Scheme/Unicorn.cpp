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

#include "Unicorn.h"
#include "parallel_for.hpp"

mat Unicorn::decimal(const mat& n) {
    return n - arma::round(n);
}

double Unicorn::compute_response(const double x, const vec& p) {
    const auto& w = p(0);
    const auto& z = p(1);
    const auto& n = p(2);

    const auto omega_r = x / w;
    const auto logw = log(omega_r);
    const auto coshlog = cosh(logw);

    return z * pow(coshlog, -2. * n - 1.);
}

vec Unicorn::compute_gradient(const double x, const vec& p) {
    vec out(num_para + 1);

    const auto& w = p(0);
    const auto& z = p(1);
    const auto& n = p(2);

    const auto omega_r = x / w;
    const auto logw = log(omega_r);
    const auto coshlog = cosh(logw);

    out(2) = pow(coshlog, -2. * n - 1.);
    out(0) = z * out(2);
    out(1) = (2. * n + 1.) * out(0) / coshlog * sinh(logw) / w;
    out(3) = -2. * out(2) * z * log(coshlog);

    return out;
}

vec Unicorn::s(const vec& p) const {
    vec sp(num_para);

    sp(0) = pow(10., min_omega + range_omega / (1. + exp(-p(0))));
    sp(1) = max_zeta / (1. + exp(-p(1)));
    sp(2) = max_order / (1. + exp(-p(2)));

    return sp;
}

vec Unicorn::ds(const vec& p) const {
    vec dsp(num_para);

    auto expp = exp(-abs(p(0)));
    dsp(0) = log(10.) * expp * pow(1. + expp, -2.) * pow(10., min_omega + range_omega / (1. + exp(-p(0)))) * range_omega;

    expp = exp(-abs(p(1)));
    dsp(1) = max_zeta * expp * pow(1. + expp, -2.);

    expp = exp(-abs(p(2)));
    dsp(2) = max_order * expp * pow(1. + expp, -2.);

    return dsp;
}

unsigned Unicorn::getSize() const { return num_para; }

double Unicorn::EvaluateWithGradient(const mat& x, mat& g) {
    mat dg(num_para * num_modes, sampling.n_cols, fill::none);
    vec n(num_modes, fill::none);
    vec dn(num_modes, fill::none);

    for(auto J = 0u; J < num_modes; ++J) {
        const vec p(&x(num_para * J), num_para);
        const auto sp = s(p);
        const auto dsp = ds(p);
        n(J) = sp(2);
        dn(J) = dsp(2);
        dd::parallel_for(0llu, sampling.n_cols, [&](const uword I) {
            const auto grad = compute_gradient(sampling(0, I), sp);
            response(J, I) = grad(0);
            vec(&dg(num_para * J, I), num_para, false, true) = grad.tail(num_para) % dsp;
        });
    }

    const rowvec fi = sum(response, 0) - sampling.row(1);

    dd::parallel_for(0llu, sampling.n_cols, [&](const uword I) { dg.col(I) *= 2. * fi(I); });

    g = sum(dg, 1);

    const vec floor_diff = decimal(n);

    g(num_para * base + 2) += 2. * weight * floor_diff % dn;

    return accu(pow(fi, 2.)) + weight * accu(pow(floor_diff, 2.));
}

size_t Unicorn::NumConstraints() const { return num_modes; }

double Unicorn::EvaluateConstraint(const size_t i, const mat& x) {
    const vec p(&x(num_para * i), num_para);

    const vec floor_diff = decimal(mat{s(p)(2)});

    return weight * floor_diff(0) * floor_diff(0);
}

void Unicorn::GradientConstraint(const size_t i, const mat& x, mat& g) {
    const vec p(&x(num_para * i), num_para);

    const vec floor_diff = decimal(mat{s(p)(2)});

    g = zeros(size(x));
    g(num_para * i + 2) = 2. * weight * floor_diff(0) * ds(p)(2);
}

QStringList Unicorn::getTypeList(const mat& result) const {
    QStringList list;

    for(auto I = 0llu; I < result.n_rows; ++I)
        list.append("Type 1 --- " + QString::number(result(I, 0)) + " " + QString::number(result(I, 1)) + " " + QString::number(result(I, 2)));

    return list;
}
