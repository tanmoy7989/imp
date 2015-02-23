/**
 *  \file IMP/isd/vonMisesKappaConjugateRestraint.h
 *  \brief Conjugate prior for \f$\kappa\f$ in the von Mises distribution.
 *
 *  Copyright 2007-2015 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPISD_VON_MISES_KAPPA_CONJUGATE_RESTRAINT_H
#define IMPISD_VON_MISES_KAPPA_CONJUGATE_RESTRAINT_H

#include <IMP/isd/isd_config.h>
#include <IMP/SingletonScore.h>
#include <IMP/kernel/Restraint.h>

IMPISD_BEGIN_NAMESPACE

//! Conjugate prior for the concentration parameter of a von Mises distribution.
/** \f[ p(\kappa|c,R_0) = \frac{\exp(\kappa R_0)}{I_0(\kappa)^c} \f]
    \f$0 < R_0 \le c\f$
    where \f$kappa\f$ is the concentration parameter.
    Default values: \f$R_0=1\f$ and \f$c=10\f$
 */
class IMPISDEXPORT vonMisesKappaConjugateRestraint : public kernel::Restraint {
  base::Pointer<kernel::Particle> kappa_;
  double old_kappaval;
  bool bessel_init_;
  double I0_, I1_;
  double c_, R0_;

 public:
  //! Create the restraint.
  vonMisesKappaConjugateRestraint(kernel::Model *m, kernel::Particle *kappa,
                                  double c = 10.0, double R0 = 0.0);

  /** This macro declares the basic needed methods: evaluate and show
   */
  virtual double unprotected_evaluate(IMP::kernel::DerivativeAccumulator *accum)
      const IMP_OVERRIDE;
  virtual IMP::kernel::ModelObjectsTemp do_get_inputs() const IMP_OVERRIDE;
  IMP_OBJECT_METHODS(vonMisesKappaConjugateRestraint);

  virtual double get_probability() const;

 private:
  void update_bessel(double kappaval);  // update memoized bessel value
  double get_kappa() const;
};

IMPISD_END_NAMESPACE

#endif /* IMPISD_VON_MISES_KAPPA_CONJUGATE_RESTRAINT_H */
