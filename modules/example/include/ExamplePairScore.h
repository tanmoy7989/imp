/**
 *  \file IMP/example/ExamplePairScore.h
 *  \brief A Score on the distance between a pair of particles.
 *
 *  Copyright 2007-2015 IMP Inventors. All rights reserved.
 */

#ifndef IMPEXAMPLE_EXAMPLE_PAIR_SCORE_H
#define IMPEXAMPLE_EXAMPLE_PAIR_SCORE_H

#include <IMP/example/example_config.h>
#include <IMP/core/XYZ.h>
#include <IMP/PairScore.h>
#include <IMP/pair_macros.h>
#include <IMP/UnaryFunction.h>
#include <IMP/base/Pointer.h>

IMPEXAMPLE_BEGIN_NAMESPACE

//! Apply a harmonic to the distance between two particles.
/** The source code is as follows:
    \include ExamplePairScore.h
    \include ExamplePairScore.cpp
*/
class IMPEXAMPLEEXPORT ExamplePairScore : public PairScore {
  double x0_, k_;

 public:
  ExamplePairScore(double x0, double k);
  virtual double evaluate_index(kernel::Model *m,
                                const kernel::ParticleIndexPair &p,
                                DerivativeAccumulator *da) const IMP_OVERRIDE;
  virtual kernel::ModelObjectsTemp do_get_inputs(
      kernel::Model *m, const kernel::ParticleIndexes &pis) const IMP_OVERRIDE;
  IMP_PAIR_SCORE_METHODS(ExamplePairScore);
  IMP_OBJECT_METHODS(ExamplePairScore);
  ;
};

IMP_OBJECTS(ExamplePairScore, ExamplePairScores);

IMPEXAMPLE_END_NAMESPACE

#endif /* IMPEXAMPLE_EXAMPLE_PAIR_SCORE_H */
