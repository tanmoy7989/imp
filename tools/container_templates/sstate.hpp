/**
 *  \file CLASSNAMEConstraint.h
 *  \brief Use a CLASSNAMEModifier applied to a PLURALVARIABLETYPE to
 *  maintain an invariant
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 */

#ifndef IMPCORE_HEADERNAME_CONSTRAINT_H
#define IMPCORE_HEADERNAME_CONSTRAINT_H

#include "core_config.h"
#include <IMP/CLASSNAMEModifier.h>
#include <IMP/Constraint.h>
#include <IMP/Particle.h>

IMP_BEGIN_NAMESPACE
// for swig
class CLASSNAMEModifier;
IMP_END_NAMESPACE

IMPCORE_BEGIN_NAMESPACE
//! Apply a CLASSNAMEFunction to a CLASSNAME
/** The score state is passed up to two CLASSNAMEModifiers, one to
    apply before evaluation and the other after. The one after
    should take a DerivativeAccumulator as its last argument for
    CLASSNAMEModifier::apply() and will only be called if
    the score was computed with derivatives.

    \see CLASSNAMEsConstraint
 */
class IMPCOREEXPORT CLASSNAMEConstraint : public Constraint
{
  IMP::internal::OwnerPointer<CLASSNAMEModifier> f_;
  IMP::internal::OwnerPointer<CLASSNAMEModifier> af_;
  STORAGETYPE v_;
public:
  /** before and after are the modifiers to apply before and after
      evaluate.
   */
  CLASSNAMEConstraint(CLASSNAMEModifier *before,
                      CLASSNAMEModifier *after, ARGUMENTTYPE vt,
                      std::string name="CLASSNAMEConstraint %1%");

  //! Apply this modifier to all the elements after an evaluate
  void set_after_evaluate_modifier(CLASSNAMEModifier* f) {
    af_=f;
  }

  //! Apply this modifier to all the elements before an evaluate
  void set_before_evaluate_modifier(CLASSNAMEModifier* f) {
    f_=f;
  }

  IMP_CONSTRAINT(CLASSNAMEConstraint);
};


IMPCORE_END_NAMESPACE

#endif  /* IMPCORE_HEADERNAME_CONSTRAINT_H */
