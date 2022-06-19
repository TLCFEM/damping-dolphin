#include "DampingMode.h"

DampingMode::DampingMode(const double in_omega, const double in_zeta, std::vector<double>&& in_p, const MT in_type)
    : type(in_type), omega_p(in_omega), zeta_p(in_zeta), p(std::forward<std::vector<double>>(in_p)) {}

void DampingMode::tidyUp() {}

DampingModeT0::DampingModeT0(const double in_omega, const double in_zeta, std::vector<double>&& in_p)
    : DampingMode(in_omega, in_zeta, std::forward<std::vector<double>>(in_p), MT::T0) {}

double DampingModeT0::operator()(const double in_omega) const {
    const auto l = in_omega < 0. ? -1. : 1.;
    const auto omega_r = abs(in_omega / omega_p);
    return zeta_p * 2. * l * omega_r / (l * omega_r * omega_r + 1.);
}

QString DampingModeT0::str() const {
    return "Type 0 --- " + QString::number(omega_p) + " " + QString::number(zeta_p);
}

QString DampingModeT0::command() const {
    return "-type0 " + QString::number(zeta_p, 'e', 5) + " " + QString::number(omega_p, 'e', 5);
}

DampingModeT1::DampingModeT1(const double in_omega, const double in_zeta, std::vector<double>&& in_p)
    : DampingMode(in_omega, in_zeta, std::forward<std::vector<double>>(in_p), MT::T1) {}

double DampingModeT1::operator()(const double in_omega) const {
    const auto l = in_omega < 0. ? -1. : 1.;
    const auto omega_r = abs(in_omega / omega_p);
    const auto n0 = 2. * l * omega_r / (l * omega_r * omega_r + 1.);
    auto n1 = pow(n0, 2. * p[0] + 1.);
    if(l < 0. && unsigned(p[0]) % 2 != 0)
        n1 = -n1;

    return zeta_p * n1;
}

void DampingModeT1::tidyUp() {
    p[0] = round(p[0]);
}

QString DampingModeT1::str() const {
    return "Type 1 --- " + QString::number(omega_p) + " " + QString::number(zeta_p) + " " + QString::number(p[0]);
}

QString DampingModeT1::command() const {
    return "-type1 " + QString::number(zeta_p, 'e', 5) + " " + QString::number(omega_p, 'e', 5) + " " + QString::number(int(p[0]));
}

DampingModeT2::DampingModeT2(const double in_omega, const double in_zeta, std::vector<double>&& in_p)
    : DampingMode(in_omega, in_zeta, std::forward<std::vector<double>>(in_p), MT::T2) {}

double DampingModeT2::operator()(const double in_omega) const {
    const auto& npr = p[0];
    const auto& npl = p[1];
    const auto r = (2. * npl + 1.) / (2. * npr + 1.);
    const auto nps = npr + npl + 1.;

    const auto l = in_omega < 0. ? -1. : 1.;
    const auto omega_r = abs(in_omega / omega_p);
    auto a = pow(omega_r, 2. * npl + 1.);
    auto b = pow(omega_r, 2. * nps);

    if(l < 0.) {
        if(unsigned(npl) % 2 == 0)
            a = -a;
        if(unsigned(nps) % 2 != 0)
            b = -b;
    }

    return zeta_p * (1. + r) * a / (1. + r * b);
}

void DampingModeT2::tidyUp() {
    p[0] = round(p[0]);
    p[1] = round(p[1]);
}

QString DampingModeT2::str() const {
    return "Type 2 --- " + QString::number(omega_p) + " " + QString::number(zeta_p) + " " + QString::number(p[0]) + " " + QString::number(p[1]);
}

QString DampingModeT2::command() const {
    return "-type2 " + QString::number(zeta_p, 'e', 5) + " " + QString::number(omega_p, 'e', 5) + " " + QString::number(int(p[0])) + " " + QString::number(int(p[1]));
}

DampingModeT3::DampingModeT3(const double in_omega, const double in_zeta, std::vector<double>&& in_p)
    : DampingMode(in_omega, in_zeta, std::forward<std::vector<double>>(in_p), MT::T3) {}

double DampingModeT3::operator()(const double in_omega) const {
    const auto& gamma = p[0];
    const auto l = in_omega < 0. ? -1. : 1.;
    const auto omega_r = abs(in_omega / omega_p);
    const auto n0 = 2. * l * omega_r / (l * omega_r * omega_r + 1.);

    return zeta_p * (1. + gamma) * n0 / (1. + gamma * l * n0 * n0);
}

QString DampingModeT3::str() const {
    return "Type 3 --- " + QString::number(omega_p) + " " + QString::number(zeta_p) + " " + QString::number(p[0], 'e', 8);
}

QString DampingModeT3::command() const {
    return "-type3 " + QString::number(zeta_p, 'e', 5) + " " + QString::number(omega_p, 'e', 5) + " " + QString::number(p[0], 'e', 7);
}

DampingModeT4::DampingModeT4(const double in_omega, const double in_zeta, std::vector<double>&& in_p)
    : DampingMode(in_omega, in_zeta, std::forward<std::vector<double>>(in_p), MT::T4) {}

double DampingModeT4::operator()(const double in_omega) const {
    const auto& npr = p[0];
    const auto& npl = p[1];
    const auto& npk = p[2];
    const auto& npm = p[3];
    const auto& gamma = p[4];
    const auto l = in_omega < 0. ? -1. : 1.;

    const auto rs = (2. * npl + 1.) / (2. * npr + 1.);
    const auto nps = npr + npl + 1.;

    const auto omega_r = abs(in_omega / omega_p);
    auto a = pow(omega_r, 2. * npl + 1.);
    auto b = pow(omega_r, 2. * nps);

    if(l < 0.) {
        if(unsigned(npl) % 2 == 0)
            a = -a;
        if(unsigned(nps) % 2 != 0)
            b = -b;
    }

    const auto ns = (1. + rs) * a / (1. + rs * b);

    const auto rp = (2. * npm + 1.) / (2. * npk + 1.);
    const auto npt = npk + npm + 1.;

    a = pow(omega_r, 2. * npm + 1.);
    b = pow(omega_r, 2. * npt);

    if(l < 0.) {
        if(unsigned(npm) % 2 == 0)
            a = -a;
        if(unsigned(npt) % 2 != 0)
            b = -b;
    }

    const auto np = (1. + rp) * a / (1. + rp * b);

    return zeta_p * (1. + gamma) * ns / (1. + l * gamma * ns * np);
}

void DampingModeT4::tidyUp() {
    p[0] = round(p[0]);
    p[1] = round(p[1]);
    p[2] = round(p[2]);
    p[3] = round(p[3]);
}

QString DampingModeT4::str() const {
    return "Type 4 --- " + QString::number(omega_p) + " " + QString::number(zeta_p) + " " + QString::number(p[0]) + " " + QString::number(p[1]) + " " + QString::number(p[2]) + " " + QString::number(p[3]) + " " + QString::number(p[4], 'e', 8);
}

QString DampingModeT4::command() const {
    return "-type4 " + QString::number(zeta_p, 'e', 5) + " " + QString::number(omega_p, 'e', 5) + " " + QString::number(int(p[0])) + " " + QString::number(int(p[1])) + " " + QString::number(int(p[2])) + " " + QString::number(int(p[3])) + " " + QString::number(p[4], 'e', 8);
}
