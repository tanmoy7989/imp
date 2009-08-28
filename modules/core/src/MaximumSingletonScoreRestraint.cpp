/**
 *  \file MaximumSingletonScoreRestraint.cpp
 *  \brief The minimum score over a container.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-9 Sali Lab. All rights reserved.
 *
 */

#include "IMP/core/MaximumSingletonScoreRestraint.h"
#include "IMP/core/internal/MinimalSet.h"
#include <IMP/internal/container_helpers.h>


IMPCORE_BEGIN_NAMESPACE

MaximumSingletonScoreRestraint
::MaximumSingletonScoreRestraint(SingletonScore *f,
                                 SingletonContainer *c,
                                 unsigned int n,
                                 std::string name):
  Restraint(name),
  f_(f), c_(c), n_(n){
}

namespace {
  typedef internal::MinimalSet<double,
    SingletonContainer::ParticleIterator, std::greater<Float> > MS;
  template <class It, class F>
  MS find_minimal_set(It b, It e, F *f, unsigned int n) {
    IMP_LOG(TERSE, "Finding lowest " << n << " of "
            << std::distance(b,e) << std::endl);
    MS bestn(n);
    for (It it= b; it != e; ++it) {
      double score
      = IMP::internal::ContainerTraits<Particle>::evaluate(f, *it, NULL);

      if (bestn.can_insert(score)) {
        bestn.insert(score, it);
        IMP_LOG(VERBOSE, "Accepeted score " << score << std::endl);
      } else {
        IMP_LOG(VERBOSE, "Rejected score " << score << std::endl);
      }
    }
    IMP_LOG_WRITE(VERBOSE, bestn.show(IMP_STREAM));
    return bestn;
  }
}

Float MaximumSingletonScoreRestraint
::evaluate(DerivativeAccumulator *da) {
  MS bestn= find_minimal_set(c_->particles_begin(),
                             c_->particles_end(), f_.get(), n_);

  double score=0;
  for (unsigned int i=0; i< bestn.size(); ++i) {
    if (da) {
      IMP::internal::ContainerTraits<Particle>::evaluate(f_.get(),
                                                     *bestn[i].second, da);
    }
    score+= bestn[i].first;
  }

  return score;
}

void MaximumSingletonScoreRestraint::show(std::ostream &out) const {
  out << "MaximumSingletonScoreRestraint over ";
  c_->show(out);
  out << " using function ";
  f_->show(out);
  out << std::endl;
}


ParticlesList MaximumSingletonScoreRestraint::get_interacting_particles() const
{
  MS bestn= find_minimal_set(c_->particles_begin(),
                             c_->particles_end(), f_.get(), n_);
  ParticlesList ret;
  for (unsigned int i=0; i< bestn.size(); ++i) {
    ParticlesTemp pt=IMP::internal
      ::ContainerTraits<Particle>::create_set(*bestn[i].second);
    if (!pt.empty()) {
      ret.push_back(pt);
    }
  }

  return ret;
}
IMPCORE_END_NAMESPACE
