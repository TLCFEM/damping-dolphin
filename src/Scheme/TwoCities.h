#ifndef TWOCITIES_H
#define TWOCITIES_H

#include "ObjectiveFunction.h"

class TwoCities : public ObjectiveFunction {
    static constexpr unsigned num_para = 4;

    static mat decimal(const mat&);

public:
    static double compute_response(double, const vec&);
    static vec compute_gradient(double, const vec&);

    [[nodiscard]] vec s(const vec&) const override;
    [[nodiscard]] vec ds(const vec&) const override;

    using ObjectiveFunction::ObjectiveFunction;

    [[nodiscard]] unsigned getSize() const override;

    double EvaluateWithGradient(const mat&, mat&) override;
    double EvaluateWithGradient(const mat&, size_t, mat&, size_t) override;

    [[nodiscard]] virtual size_t NumConstraints() const override;

    double EvaluateConstraint(size_t, const mat&) override;
    void GradientConstraint(size_t, const mat&, mat&) override;

    [[nodiscard]] QStringList getTypeList(const mat&) const override;
};

#endif // TWOCITIES_H
