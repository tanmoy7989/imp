/**
 *  \file SingletonRestraint.h
 *  \brief Apply a SingletonScore to a Particle.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-9 Sali Lab. All rights reserved.
 *
 */

#ifndef IMPCORE_SINGLETON_RESTRAINT_H
#define IMPCORE_SINGLETON_RESTRAINT_H

#include "config.h"
#include "internal/version_info.h"

#include <IMP/Restraint.h>
#include <IMP/Pointer.h>
#include <IMP/SingletonScore.h>

#include <iostream>

IMPCORE_BEGIN_NAMESPACE

//! Applies a SingletonScore to a Particle.
/** This restraint stores a Particle.
    \see SingletonRestraint
 */
class IMPCOREEXPORT SingletonRestraint : public Restraint
{
  Pointer<SingletonScore> ss_;
  Particle* v_;
public:
  //! Create the restraint.
  /** This function takes the function to apply to the
      stored Singleton and the Singleton.
   */
  SingletonRestraint(SingletonScore *ss,
                      Particle *a);

  IMP_RESTRAINT(SingletonRestraint, internal::version_info);

  virtual ParticlesList get_interacting_particles() const;
};

IMPCORE_END_NAMESPACE

#endif  /* IMPCORE_SINGLETON_RESTRAINT_H */
