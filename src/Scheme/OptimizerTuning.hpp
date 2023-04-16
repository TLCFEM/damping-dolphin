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

#ifndef OPTIMIZERTUNING_H
#define OPTIMIZERTUNING_H

#include "ObjectiveFunction.h"

struct OptimizerSetting {
    int maxOrder = 5;
    int batchSize = 32;
    int maxIter = 20000;
    double tolerance = 1E-8;
    double stepSize = 1E-3;
    double weight = 1E-4;
};

template<typename T> void StepSize(T& optimizer, const double step_size) {
    optimizer.StepSize() = step_size;
}

template<> void StepSize(KatyushaProximal&, double) {
}
template<> void StepSize(L_BFGS&, double) {
}
template<> void StepSize(AugLagrangian&, double) {
}
template<> void StepSize(SA<>&, double) {
}
template<> void StepSize(CNE&, double) {
}
template<> void StepSize(DE&, double) {
}

template<typename T> void Tolerance(T& optimizer, const double tolerance) {
    optimizer.Tolerance() = tolerance;
}

template<> void Tolerance(L_BFGS&, double) {
}
template<> void Tolerance(AugLagrangian&, double) {
}
template<> void Tolerance(SPSA&, double) {
}

template<typename T> void BatchSize(T& optimizer, const int batchSize) {
    optimizer.BatchSize() = batchSize;
}

template<> void BatchSize(L_BFGS&, int) {
}
template<> void BatchSize(GradientDescent&, int) {
}
template<> void BatchSize(AugLagrangian&, int) {
}
template<> void BatchSize(SA<>&, int) {
}
template<> void BatchSize(CNE&, int) {
}
template<> void BatchSize(DE&, int) {
}
template<> void BatchSize(SPSA&, int) {
}

template<typename T> void MaxIterations(T& optimizer, const int maxIter) {
    optimizer.MaxIterations() = maxIter;
}

template<> void MaxIterations(CNE& optimizer, const int maxIter) {
    optimizer.MaxGenerations() = maxIter;
}
template<> void MaxIterations(DE& optimizer, const int maxIter) {
    optimizer.MaxGenerations() = maxIter;
}

template<typename MatType>
class EarlyQuit {
    std::atomic<bool>* if_quit;

public:
    explicit EarlyQuit(std::atomic<bool>* quit)
        : if_quit(quit) {}

    template<typename OptimizerType, typename FunctionType>
    bool BeginOptimization(OptimizerType&,
                           FunctionType&,
                           const MatType&) { return *if_quit; }

    template<typename OptimizerType, typename FunctionType>
    bool Evaluate(OptimizerType&,
                  FunctionType&,
                  const MatType&,
                  double) { return *if_quit; }

    template<typename OptimizerType, typename FunctionType, typename GradType>
    bool Gradient(OptimizerType&,
                  FunctionType&,
                  const MatType&,
                  const GradType&) { return *if_quit; }

    template<typename OptimizerType, typename FunctionType>
    bool StepTaken(OptimizerType&,
                   FunctionType&,
                   const MatType&) { return *if_quit; }
};

template<typename T> mat run_optimizer(const OptimizerSetting& opt_setting, ObjectiveFunction* f, std::atomic<bool>* quit) {
    T optimizer;
    StepSize(optimizer, opt_setting.stepSize);
    Tolerance(optimizer, opt_setting.tolerance);
    BatchSize(optimizer, opt_setting.batchSize);
    MaxIterations(optimizer, opt_setting.maxIter);

    f->setWeight(opt_setting.weight);
    f->setMaxOrder(opt_setting.maxOrder);

    mat x = 2. * randn(f->getSize() * f->getNumberModes());

    optimizer.Optimize(*f, x, Report(0.1), PrintLoss(), EarlyQuit<arma::mat>(quit));

    return reshape(x, f->getSize(), f->getNumberModes()).eval().each_col([&](vec& a) { a = f->s(a); }).t();
}

#endif // OPTIMIZERTUNING_H
