/**
 *  \file CLASSNAMEFilter.h    \brief A filter for CLASSNAMEs.
 *
 *  This file is generated by a script (core/tools/make-containers).
 *  Do not edit directly.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 */

#ifndef IMP_HEADERNAME_FILTER_H
#define IMP_HEADERNAME_FILTER_H

#include "kernel_config.h"
#include "base_types.h"
#include "macros.h"
#include "VersionInfo.h"
#include "ParticleTuple.h"
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>


IMP_BEGIN_NAMESPACE


//! A shared filter for CLASSNAMEs
/** Stores a searchable shared collection of CLASSNAMEs.
    \ingroup restraints

    Implementors should see IMP_HEADERNAME_FILTER().
 */
class IMPEXPORT CLASSNAMEFilter : public Object
{
public:
  CLASSNAMEFilter(std::string name="CLASSNAMEFilter %1%");

  /** \note This function may be linear. Be aware of the complexity
      bounds of your particular filter.
   */
  virtual bool get_contains_FUNCTIONNAME(ARGUMENTTYPE p) const =0;

  /** \name Interactions
      Return the set of particles used when applied to the passed
      list.
      @{
  */
  virtual ParticlesTemp get_input_particles(Particle* p) const=0;
  virtual ContainersTemp get_input_containers(Particle* p) const=0;
  /** @} */

#if !defined(IMP_DOXYGEN) && !defined(SWIG)
  virtual void filter_in_place(PLURALVARIABLETYPE& ps) const;
#endif

  IMP_REF_COUNTED_DESTRUCTOR(CLASSNAMEFilter);
};

IMP_OUTPUT_OPERATOR(CLASSNAMEFilter);

IMP_OBJECTS(CLASSNAMEFilter,CLASSNAMEFilters);

IMP_END_NAMESPACE

#endif  /* IMP_HEADERNAME_FILTER_H */
