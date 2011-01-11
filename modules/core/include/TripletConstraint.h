/**
 *  \file TripletConstraint.h
 *  \brief Use a TripletModifier applied to a ParticleTripletsTemp to
 *  maintain an invariant
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 */

#ifndef IMPCORE_TRIPLET_CONSTRAINT_H
#define IMPCORE_TRIPLET_CONSTRAINT_H

#include "core_config.h"
#include <IMP/TripletModifier.h>
#include <IMP/Constraint.h>
#include <IMP/Particle.h>

IMP_BEGIN_NAMESPACE
// for swig
class TripletModifier;
IMP_END_NAMESPACE

IMPCORE_BEGIN_NAMESPACE
//! Apply a TripletFunction to a Triplet
/** The score state is passed up to two TripletModifiers, one to
    apply before evaluation and the other after. The one after
    should take a DerivativeAccumulator as its last argument for
    TripletModifier::apply() and will only be called if
    the score was computed with derivatives.

    \see TripletsConstraint
 */
class IMPCOREEXPORT TripletConstraint : public Constraint
{
  IMP::internal::OwnerPointer<TripletModifier> f_;
  IMP::internal::OwnerPointer<TripletModifier> af_;
  ParticleTriplet v_;
public:
  /** before and after are the modifiers to apply before and after
      evaluate.
   */
  TripletConstraint(TripletModifier *before,
                      TripletModifier *after, const ParticleTriplet& vt,
                      std::string name="TripletConstraint %1%");

  //! Apply this modifier to all the elements after an evaluate
  void set_after_evaluate_modifier(TripletModifier* f) {
    af_=f;
  }

  //! Apply this modifier to all the elements before an evaluate
  void set_before_evaluate_modifier(TripletModifier* f) {
    f_=f;
  }

  IMP_CONSTRAINT(TripletConstraint);
};


IMPCORE_END_NAMESPACE

#endif  /* IMPCORE_TRIPLET_CONSTRAINT_H */
