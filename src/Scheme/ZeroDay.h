#ifndef ZERODAY_H
#define ZERODAY_H

#include "ObjectiveFunction.h"

class ZeroDay : public ObjectiveFunction {
    static constexpr unsigned num_para = 2;

public:
    static double compute_response(double, const vec&);
    static vec compute_gradient(double, const vec&);

    [[nodiscard]] vec s(const vec&) const override;
    [[nodiscard]] vec ds(const vec&) const override;

    using ObjectiveFunction::ObjectiveFunction;

    [[nodiscard]] unsigned getSize() const override;

    double EvaluateWithGradient(const mat&, mat&) override;
    double EvaluateWithGradient(const mat&, size_t, mat&, size_t) override;

    [[nodiscard]] QStringList getTypeList(const mat&) const override;
};

#endif // ZERODAY_H
