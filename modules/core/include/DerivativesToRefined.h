/**
 *  \file DerivativesToRefined.h
 *  \brief Accumulate the derivatives of the refined particles
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 */

#ifndef IMPCORE_DERIVATIVES_TO_REFINED_H
#define IMPCORE_DERIVATIVES_TO_REFINED_H

#include "core_config.h"

#include "XYZ.h"
#include <IMP/Refiner.h>
#include <IMP/Pointer.h>
#include <IMP/SingletonModifier.h>

IMPCORE_BEGIN_NAMESPACE

//! Copy the derivatives from the particle to its refined particles.
/** \see DerivatvesFromRefinedSingletonModifier
    \see CoverRefined
    \see CentroidOfRefinedSingletonModifer
 An example showing a how to use such a score state to maintain a cover
 of the atoms of a protein by a sphere per residue.
 \pythonexample{cover_particles}
 */
class IMPCOREEXPORT DerivativesToRefined:
public SingletonDerivativeModifier
{
  IMP::internal::OwnerPointer<Refiner> refiner_;
  FloatKeys ks_;
public:
  //! Copy ks to the particles returned by r.
  DerivativesToRefined(Refiner *r,
                       FloatKeys ks
                       = XYZ::get_xyz_keys());

  IMP_SINGLETON_DERIVATIVE_MODIFIER(DerivativesToRefined);
};

IMPCORE_END_NAMESPACE

#endif  /* IMPCORE_DERIVATIVES_TO_REFINED_H */
