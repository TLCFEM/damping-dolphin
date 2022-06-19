#ifndef OBJECTIVEFUNCTION_H
#define OBJECTIVEFUNCTION_H

#include "../damping-dolphin.h"

class ObjectiveFunction {
protected:
    const unsigned num_modes;

    mat sampling, response;

    double min_omega = 0., max_omega = 0., min_zeta = 0., max_zeta = 0.;
    double range_omega = 0.;

    uvec base;

    double weight = 0.;

    int max_order = 10;

public:
    [[nodiscard]] virtual vec s(const vec&) const;
    [[nodiscard]] virtual vec ds(const vec&) const;

    explicit ObjectiveFunction(unsigned);

    void initializeSampling(mat&&);

    void setWeight(double);
    void setMaxOrder(int);

    [[nodiscard]] virtual unsigned getSize() const = 0;
    [[nodiscard]] unsigned getNumberModes() const;

    [[nodiscard]] virtual size_t NumConstraints() const;
    [[nodiscard]] virtual size_t NumFunctions() const;
    virtual void Shuffle();

    virtual double Evaluate(const mat&);
    virtual void Gradient(const mat&, mat&);

    virtual double EvaluateConstraint(size_t, const mat&);
    virtual void GradientConstraint(size_t, const mat&, mat&);

    virtual double Evaluate(const mat&, size_t, size_t);
    virtual void Gradient(const mat&, size_t, mat&, size_t);

    virtual double EvaluateWithGradient(const mat&, mat&) = 0;
    virtual double EvaluateWithGradient(const mat&, size_t, mat&, size_t) = 0;

    [[nodiscard]] virtual QStringList getTypeList(const mat&) const = 0;
};

#endif // OBJECTIVEFUNCTION_H
