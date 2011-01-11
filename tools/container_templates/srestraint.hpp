/**
 *  \file CLASSNAMERestraint.h
 *  \brief Apply a CLASSNAMEScore to a CLASSNAME.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPCORE_HEADERNAME_RESTRAINT_H
#define IMPCORE_HEADERNAME_RESTRAINT_H

#include "core_config.h"

#include <IMP/Restraint.h>
#include <IMP/Pointer.h>
#include <IMP/CLASSNAMEScore.h>
#include "internal/HELPERNAME_helpers.h"

#include <iostream>

IMPCORE_BEGIN_NAMESPACE

//! Applies a CLASSNAMEScore to a CLASSNAME.
/** This restraint stores a CLASSNAME.
    \see CLASSNAMERestraint
 */
class IMPCOREEXPORT CLASSNAMERestraint :
  public CLASSNAMEScoreRestraint
{
  IMP::internal::OwnerPointer<CLASSNAMEScore> ss_;
  STORAGETYPE v_;
  mutable double score_;
public:
  //! Create the restraint.
  /** This function takes the function to apply to the
      stored CLASSNAME and the CLASSNAME.
   */
  CLASSNAMERestraint(CLASSNAMEScore *ss,
                     ARGUMENTTYPE vt,
                     std::string name="CLASSNAMERestraint %1%");

  CLASSNAMEScore* get_score() const {
    return ss_;
  }
  VARIABLETYPE get_argument() const {
    return v_;
  }

  IMP_INCREMENTAL_RESTRAINT(CLASSNAMERestraint);
};

IMPCORE_END_NAMESPACE

#endif  /* IMPCORE_HEADERNAME_RESTRAINT_H */
