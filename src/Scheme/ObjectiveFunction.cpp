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

void ObjectiveFunction::setWeight(const double W) {
    weight = W;
}

void ObjectiveFunction::setMaxOrder(const int M) {
    max_order = M;
}

unsigned ObjectiveFunction::getNumberModes() const {
    return num_modes;
}

double ObjectiveFunction::Evaluate(const mat& x) {
    mat g;

    return EvaluateWithGradient(x, g);
}

void ObjectiveFunction::Gradient(const mat& x, mat& g) { EvaluateWithGradient(x, g); }

size_t ObjectiveFunction::NumConstraints() const { return 0; }

double ObjectiveFunction::EvaluateConstraint(const size_t, const arma::mat&) { return 0.; }

void ObjectiveFunction::GradientConstraint(const size_t, const arma::mat& x, arma::mat& g) {
    g.zeros(size(x));
}

void ObjectiveFunction::Shuffle() {
    const uvec ordering = randperm(sampling.n_cols);

    sampling = sampling.cols(ordering);
    response = response.cols(ordering);
}

size_t ObjectiveFunction::NumFunctions() const { return sampling.n_cols; }

double ObjectiveFunction::Evaluate(const mat& x, const size_t i, const size_t batchSize) {
    mat g;

    return EvaluateWithGradient(x, i, g, batchSize);
}

void ObjectiveFunction::Gradient(const mat& x, const size_t i, mat& g, const size_t batchSize) {
    EvaluateWithGradient(x, i, g, batchSize);
}
