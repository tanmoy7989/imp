/**
 *  \file MinimumTripletRestraint.cpp
 *  \brief The minimum score over a container.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 *
 */

#include "IMP/container/MinimumTripletRestraint.h"
#include "IMP/algebra/internal/MinimalSet.h"
#include <IMP/internal/container_helpers.h>


IMPCONTAINER_BEGIN_NAMESPACE

MinimumTripletRestraint
::MinimumTripletRestraint(TripletScore *f,
                           TripletContainer *c,
                           unsigned int n,
                           std::string name):
  Restraint(name),
  f_(f), c_(c), n_(n){
}

namespace {
  typedef algebra::internal::MinimalSet<double,
          ParticleTriplet, std::less<double> > MS;
  template <class It, class F>
  MS find_minimal_set(It b, It e, F *f, unsigned int n) {
    IMP_LOG(TERSE, "Finding Minimum " << n << " of "
            << std::distance(b,e) << std::endl);
    MS bestn(n);
    for (It it= b; it != e; ++it) {
      double score= f->evaluate(*it, NULL);

      if (bestn.can_insert(score)) {
        bestn.insert(score, *it);
      }
    }
    return bestn;
  }
}

double MinimumTripletRestraint
::unprotected_evaluate(DerivativeAccumulator *da) const {
  MS bestn= find_minimal_set(c_->particle_triplets_begin(),
                             c_->particle_triplets_end(), f_.get(), n_);

  double score=0;
  for (unsigned int i=0; i< bestn.size(); ++i) {
    if (da) {
      f_->evaluate(bestn[i].second, da);
    }
    score+= bestn[i].first;
  }

  return score;
}

void MinimumTripletRestraint::do_show(std::ostream &out) const {
  out << "container " << *c_ << std::endl;
  out << "score " << *f_ << std::endl;
}


ParticlesTemp MinimumTripletRestraint::get_input_particles() const
{
  return IMP::internal::get_input_particles(f_.get(),
                                            c_->get_contained_particles());
}

ContainersTemp MinimumTripletRestraint::get_input_containers() const
{
  return ContainersTemp(1, c_);
}


IMPCONTAINER_END_NAMESPACE
