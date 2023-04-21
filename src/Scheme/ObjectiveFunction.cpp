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

#include "ObjectiveFunction.h"

vec ObjectiveFunction::s(const vec& p) const { return p; }

vec ObjectiveFunction::ds(const vec& p) const { return ones(size(p)); }

ObjectiveFunction::ObjectiveFunction(const unsigned S)
    : num_modes(S) {}

void ObjectiveFunction::initializeSampling(mat&& T) {
    sampling = std::forward<mat>(T);
    response.set_size(num_modes, sampling.n_cols);
    min_omega = log10(min(sampling.row(0))) - .1;
    max_omega = log10(max(sampling.row(0))) + .1;
    min_zeta = min(sampling.row(1));
    max_zeta = max(sampling.row(1));
    range_omega = max_omega - min_omega;

    base = linspace<uvec>(0, num_modes - 1, num_modes);
}

void ObjectiveFunction::setWeight(const double W) { weight = W; }

void ObjectiveFunction::setMaxOrder(const int M) { max_order = M; }

unsigned ObjectiveFunction::getNumberModes() const { return num_modes; }

double ObjectiveFunction::Evaluate(const mat& x) {
    mat g;

    return EvaluateWithGradient(x, g);
}

void ObjectiveFunction::Gradient(const mat& x, mat& g) { EvaluateWithGradient(x, g); }

size_t ObjectiveFunction::NumConstraints() const { return 0; }

double ObjectiveFunction::EvaluateConstraint(const size_t, const arma::mat&) { return 0.; }

void ObjectiveFunction::GradientConstraint(const size_t, const arma::mat& x, arma::mat& g) { g.zeros(size(x)); }
