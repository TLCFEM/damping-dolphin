#include "DampingCurve.h"
#include "DampingMode.h"

void DampingCurve::initializeVector(const size_t samples) {
    const auto size = static_cast<int>(samples);
    omega.resize(size);
    zeta_sum.resize(size);
    zeta.resize(damping_modes.size());
    for(auto& I : zeta)
        I.resize(size);
}

void DampingCurve::computeCurve() {
    for(auto j = 0; j < damping_modes.size(); ++j)
        for(auto i = 0; i < omega.size(); ++i) {
            zeta[j][i] = damping_modes[j]->operator()(omega[i]);
            zeta_sum[i] += zeta[j][i];
        }
}

void DampingCurve::addMode(std::unique_ptr<DampingMode>&& new_mode) {
    damping_modes.push_back(std::forward<std::unique_ptr<DampingMode>>(new_mode));
}

void DampingCurve::removeMode(const int tag) {
    if(-1 == tag) {
        damping_modes.clear();
        return;
    }

    if(tag < damping_modes.size()) {
        damping_modes.erase(damping_modes.begin() + tag);
        return;
    }
}

void DampingCurve::updateLinearDampingCurve(const double start, const double end, const size_t samples) {
    initializeVector(samples);

    const auto gap = end - start;

    for(int i = 0; i < static_cast<int>(samples); ++i) {
        omega[i] = double(i) / (static_cast<double>(samples) - 1.) * gap + start;
        zeta_sum[i] = 0.;
    }

    computeCurve();
}

void DampingCurve::updateLogarithmicDampingCurve(double start, double end, const size_t samples) {
    initializeVector(samples);

    start = log10(start);
    end = log10(end);

    const auto gap = end - start;

    for(int i = 0; i < static_cast<int>(samples); ++i) {
        omega[i] = pow(10., double(i) / (static_cast<double>(samples) - 1.) * gap + start);
        zeta_sum[i] = 0.;
    }

    computeCurve();
}

double DampingCurve::query(const double in_omega) {
    double out_zeta = 0.;

    for(const auto& I : qAsConst(damping_modes))
        out_zeta += I->operator()(in_omega);

    return out_zeta;
}

const QVector<double>& DampingCurve::getFrequencyVector() {
    return omega;
}

const QVector<double>& DampingCurve::getDampingRatioVector(const int tag) {
    if(-1 == tag) return zeta_sum;

    return zeta[tag];
}

QStringList DampingCurve::getTypeInfo() {
    QStringList type_list;

    for(const auto& I : qAsConst(damping_modes))
        type_list.append(I->str());

    return type_list;
}

QStringList DampingCurve::getCommand() {
    QStringList command_list;

    for(const auto& I : qAsConst(damping_modes))
        command_list.append(I->command());

    return command_list;
}

int DampingCurve::count() {
    return damping_modes.size();
}

double DampingCurve::minFrequency() {
    return *std::min_element(omega.cbegin(), omega.cend());
}

double DampingCurve::maxFrequency() {
    return *std::max_element(omega.cbegin(), omega.cend());
}

double DampingCurve::minDampingRatio() {
    auto min_zeta = 0.;

    for(const auto& I : qAsConst(zeta))
        min_zeta = std::min(min_zeta, *std::min_element(I.cbegin(), I.cend()));

    min_zeta = std::min(min_zeta, *std::min_element(zeta_sum.cbegin(), zeta_sum.cend()));

    return std::max(min_zeta, -1.);
}

double DampingCurve::maxDampingRatio() {
    auto max_zeta = 0.;

    for(const auto& I : qAsConst(zeta))
        max_zeta = std::max(max_zeta, *std::max_element(I.cbegin(), I.cend()));

    max_zeta = std::max(max_zeta, *std::max_element(zeta_sum.cbegin(), zeta_sum.cend()));

    return std::min(max_zeta, 1.);
}

void DampingCurve::tidyUp() {
    for(auto& I : damping_modes)
        I->tidyUp();
}

void ControlPoint::addPoint(const double in_omega, const double in_zeta) {
    omega.push_back(in_omega);
    zeta.push_back(in_zeta);
}

void ControlPoint::removePoint(const int tag) {
    if(-1 == tag) {
        omega.clear();
        zeta.clear();
    }
    else {
        omega.erase(omega.begin() + tag);
        zeta.erase(zeta.begin() + tag);
    }
}

const QVector<double>& ControlPoint::getFrequencyVector() {
    return omega;
}

const QVector<double>& ControlPoint::getDampingRatioVector() {
    return zeta;
}

double ControlPoint::minFrequency() {
    return *std::min_element(omega.cbegin(), omega.cend());
}

double ControlPoint::maxFrequency() {
    return *std::max_element(omega.cbegin(), omega.cend());
}

double ControlPoint::minDampingRatio() {
    return *std::min_element(zeta.cbegin(), zeta.cend());
}

double ControlPoint::maxDampingRatio() {
    return *std::max_element(zeta.cbegin(), zeta.cend());
}

mat ControlPoint::getSampling() {
    mat sampling(count(), 2);

    for(auto I = 0; I < count(); ++I) {
        sampling(I, 0) = omega[I];
        sampling(I, 1) = zeta[I];
    }

    const uvec ordering = sort_index(sampling.col(0));

    return sampling.rows(ordering);
}

int ControlPoint::count() {
    return omega.size();
}
