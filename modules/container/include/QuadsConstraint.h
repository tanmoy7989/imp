/**
 *  \file QuadsConstraint.h
 *  \brief Use a QuadModifier applied to a ParticleQuadsTemp to
 *  maintain an invariant
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 */

#ifndef IMPCONTAINER_QUADS_CONSTRAINT_H
#define IMPCONTAINER_QUADS_CONSTRAINT_H

#include "container_config.h"
#include <IMP/QuadContainer.h>
#include <IMP/QuadModifier.h>
#include <IMP/Constraint.h>

IMP_BEGIN_NAMESPACE
// for swig
class QuadContainer;
class QuadModifier;
IMP_END_NAMESPACE

IMPCONTAINER_BEGIN_NAMESPACE
//! Apply a QuadFunction to a QuadContainer to maintain an invariant
/** The score state is passed up to two QuadModifiers, one to
    apply before evaluation and the other after. The one after
    should take a DerivativeAccumulator as its last argument for
    QuadModifier::apply() and will only be called if
    the score was computed with derivatives.

    An example showing a how to use such a score state to maintain a cover
    of the atoms of a protein by a sphere per residue.
    \verbinclude cover_particles.py

    \see core::QuadConstraint
 */
class IMPCONTAINEREXPORT QuadsConstraint : public Constraint
{
  IMP::internal::OwnerPointer<QuadModifier> f_;
  IMP::internal::OwnerPointer<QuadModifier> af_;
  IMP::internal::OwnerPointer<QuadContainer> c_;
public:
  /** \param[in] c The Container to hold the elements to process
      \param[in] before The QuadModifier to apply to all elements
      before evaluate.
      \param[in] after The QuadModifier to apply to all elements
      after evaluate.
      \param[in] name The object name
   */
  QuadsConstraint(QuadContainer *c, QuadModifier *before,
                       QuadModifier *after,
                       std::string name="QuadConstraint %1%");

  //! Apply this modifier to all the elements after an evaluate
  void set_after_evaluate_modifier(QuadModifier* f) {
    af_=f;
  }

  //! Apply this modifier to all the elements before an evaluate
  void set_before_evaluate_modifier(QuadModifier* f) {
    f_=f;
  }

  IMP_CONSTRAINT(QuadsConstraint);
};

IMP_OBJECTS(QuadsConstraint,QuadsConstraints);


IMPCONTAINER_END_NAMESPACE

#endif  /* IMPCONTAINER_QUADS_CONSTRAINT_H */
