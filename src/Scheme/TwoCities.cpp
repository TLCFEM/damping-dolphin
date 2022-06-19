#include "TwoCities.h"
#include "parallel_for.hpp"

mat TwoCities::decimal(const mat& n) {
    return n - arma::round(n);
}

double TwoCities::compute_response(const double x, const vec& p) {
    const auto& w = p(0);
    const auto& z = p(1);
    const auto& nr = p(2);
    const auto& nl = p(3);

    const auto ra = 2. * nl + 1.;
    const auto rb = 2. * nr + 1.;
    const auto r = ra / rb;
    const auto xr = x / w;

    return z * (1. + r) * pow(xr, 2. * nl + 1.) / (1. + r * pow(xr, 2. * (1. + nr + nl)));
}

vec TwoCities::compute_gradient(double x, const vec& p) {
    vec out(num_para + 1);

    const auto& w = p(0);
    const auto& z = p(1);
    const auto& nr = p(2);
    const auto& nl = p(3);

    const auto xr = x / w;
    const auto nps = 1. + nr + nl;
    const auto ra = 2. * nl + 1.;
    const auto rb = 2. * nr + 1.;
    const auto r = ra / rb;

    const auto fa = (1. + r) * pow(xr, 2. * nl + 1.);
    const auto fb = 1. + r * pow(xr, 2. * nps);

    const auto fc = pow(xr, ra);
    const auto fd = pow(nr + .5, 2.);
    const auto fe = pow(xr, 2. * nps);

    const auto aw = -2. * fc * ra * nps / (w * rb);
    const auto anr = -fc * (1. * nl + .5) / fd;
    const auto anl = fc * (4. * nps * log(xr) + 2.) / rb;

    const auto bw = -2. * fe * ra * nps / (w * rb);
    const auto bnr = fe * ra * (2. * fd * log(xr) - nr - .5) / (fd * rb);
    const auto bnl = 2. * fe * (ra * log(xr) + 1.) / rb;

    out(2) = fa / fb;
    out(0) = z * out(2);
    out(1) = z / fb * (aw - out(2) * bw);
    out(3) = z / fb * (anr - out(2) * bnr);
    out(4) = z / fb * (anl - out(2) * bnl);

    return out;
}

vec TwoCities::s(const vec& p) const {
    vec sp(num_para);

    sp(0) = pow(10., min_omega + range_omega / (1. + exp(-p(0))));
    sp(1) = max_zeta / (1. + exp(-p(1)));
    sp(2) = max_order / (1. + exp(-p(2)));
    sp(3) = max_order / (1. + exp(-p(3)));

    return sp;
}

vec TwoCities::ds(const vec& p) const {
    vec dsp(num_para);

    auto expp = exp(-abs(p(0)));
    dsp(0) = log(10.) * expp * pow(1. + expp, -2.) * pow(10., min_omega + range_omega / (1. + exp(-p(0)))) * range_omega;

    expp = exp(-abs(p(1)));
    dsp(1) = max_zeta * expp * pow(1. + expp, -2.);

    expp = exp(-abs(p(2)));
    dsp(2) = max_order * expp * pow(1. + expp, -2.);

    expp = exp(-abs(p(3)));
    dsp(3) = max_order * expp * pow(1. + expp, -2.);

    return dsp;
}

unsigned TwoCities::getSize() const {
    return num_para;
}

double TwoCities::EvaluateWithGradient(const mat& x, mat& g) {
    mat dg(num_para * num_modes, sampling.n_cols, fill::none);
    mat n(2, num_modes, fill::none);
    mat dn(2, num_modes, fill::none);

    for(auto J = 0u; J < num_modes; ++J) {
        const vec p(&x(num_para * J), num_para);
        const auto sp = s(p);
        const auto dsp = ds(p);
        n.col(J) = sp.tail(2);
        dn.col(J) = dsp.tail(2);
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

    const mat floor_diff = decimal(n).t();

    g(num_para * base + 2) += 2. * weight * floor_diff.col(0) % dn.row(0).t();
    g(num_para * base + 3) += 2. * weight * floor_diff.col(1) % dn.row(1).t();

    return accu(pow(fi, 2.)) + weight * accu(pow(floor_diff, 2.));
}

double TwoCities::EvaluateWithGradient(const mat& x, const size_t i, mat& g, const size_t batchSize) {
    mat dg(num_para * num_modes, batchSize, fill::none);
    mat n(2, num_modes, fill::none);
    mat dn(2, num_modes, fill::none);

    for(auto J = 0u; J < num_modes; ++J) {
        const vec p(&x(num_para * J), num_para);
        const auto sp = s(p);
        const auto dsp = ds(p);
        n.col(J) = sp.tail(2);
        dn.col(J) = dsp.tail(2);
        dd::parallel_for(size_t(0), batchSize, [&](const size_t I) {
            const auto grad = compute_gradient(sampling(0, I + i), sp);
            response(J, I + i) = grad(0);
            vec gi(&dg(num_para * J, I), num_para, false, true);
            gi = grad.tail(num_para) % dsp;
        });
    }

    const rowvec fi = sum(response.cols(i, i + batchSize - 1), 0) - sampling.row(1).cols(i, i + batchSize - 1);

    dd::parallel_for(size_t(0), batchSize, [&](const size_t I) { dg.col(I) *= 2. * fi(I); });

    g = sum(dg, 1);

    const mat floor_diff = decimal(n).t();

    g(num_para * base + 2) += 2. * weight * floor_diff.col(0) % dn.row(0).t();
    g(num_para * base + 3) += 2. * weight * floor_diff.col(1) % dn.row(1).t();

    return accu(pow(fi, 2.)) + weight * accu(pow(floor_diff, 2.));
}

size_t TwoCities::NumConstraints() const {
    return 2 * num_modes;
}

double TwoCities::EvaluateConstraint(const size_t i, const mat& x) {
    const vec p(&x(num_para * (i / 2)), num_para);

    const vec floor_diff = decimal(mat{s(p)(i % 2 + 2)});

    return weight * floor_diff(0) * floor_diff(0);
}

void TwoCities::GradientConstraint(const size_t i, const mat& x, mat& g) {
    const auto i_mode = i / 2;
    const auto i_shift = i % 2 + 2;

    const vec p(&x(num_para * i_mode), num_para);

    const vec floor_diff = decimal(mat{s(p)(i_shift)});

    g = zeros(size(x));
    g(num_para * i_mode + i_shift) = 2. * weight * floor_diff(0) * ds(p)(i_shift);
}

QStringList TwoCities::getTypeList(const mat& result) const {
    QStringList list;

    for(auto I = 0llu; I < result.n_rows; ++I)
        list.append("Type 2 --- " + QString::number(result(I, 0)) + " " + QString::number(result(I, 1)) + " " + QString::number(result(I, 2)) + " " + QString::number(result(I, 3)));

    return list;
}
