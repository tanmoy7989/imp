/**
 *  \file RefineOncePairScore.cpp
 *  \brief Refine particles at most once with a ParticleRefiner.
 *
 *
 *  Copyright 2007-9 Sali Lab. All rights reserved.
 */

#include <IMP/misc/RefineOncePairScore.h>

#include <IMP/core/bond_decorators.h>
#include <IMP/core/XYZDecorator.h>
#include <IMP/deprecation.h>

#include <cmath>

IMPMISC_BEGIN_NAMESPACE

#ifndef IMP_NO_DEPRECATED

RefineOncePairScore::RefineOncePairScore(ParticleRefiner *r,
                                         PairScore *f): r_(r), f_(f) {
  IMP_DEPRECATED(RefineOncePairScore, RefinedPairsPairScore);
}

Float RefineOncePairScore::evaluate(Particle *a, Particle *b,
                                    DerivativeAccumulator *da) const
{
  Particle* p[2]={a,b};
  Particles ps[2];
  for (unsigned int i=0; i< 2; ++i) {
    if (r_->get_can_refine(p[i])) {
      ps[i]= r_->get_refined(p[i]);
    } else {
      ps[i].push_back(p[i]);
    }
    IMP_LOG(VERBOSE, "Refining " << p[i]->get_name()
            << " resulted in " << ps[i].size() << " particles"
            << std::endl);
  }

  Float ret=0;
  for (unsigned int i=0; i< ps[0].size(); ++i) {
    for (unsigned int j=0; j< ps[1].size(); ++j) {
      ret+=f_->evaluate(ps[0][i], ps[1][j], da);
    }
  }

  for (unsigned int i=0; i< 2; ++i) {
    if (ps[i].size() != 1 || (ps[i].size()==1 && ps[i][0] != p[i])) {
      IMP_LOG(VERBOSE, "Refining " << p[i]->get_name()
              << " resulted in " << ps[i].size() << " particles"
              << std::endl);
      r_->cleanup_refined(p[i], ps[i], da);
    }
  }
  return ret;
}

void RefineOncePairScore::show(std::ostream &out) const
{
  out << "RefineOncePairScore using ";
  f_->show(out);
  r_->show(out);
}

#endif // IMP_NO_DEPRECATED

IMPMISC_END_NAMESPACE
