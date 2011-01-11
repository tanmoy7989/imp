/**
 *  \file MaximumSingletonScore.h    \brief Define SingletonScore.
 *
 *  This file is generated by a script (tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 */

#ifndef IMPCONTAINER_MAXIMUM_SINGLETON_SCORE_H
#define IMPCONTAINER_MAXIMUM_SINGLETON_SCORE_H

#include "container_config.h"
#include <IMP/SingletonScore.h>

IMPCONTAINER_BEGIN_NAMESPACE

//! Evaluate the min or max n particle scores of the passed set
/** Each of the set of SingletonScores is evaluated and the sum of the
    minimum n is returned.
*/
class IMPCONTAINEREXPORT MaximumSingletonScore : public SingletonScore
{
  SingletonScores scores_;
  unsigned int n_;
public:
  MaximumSingletonScore(const SingletonScoresTemp &scores,
                       unsigned int n=1,
                       std::string name="SingletonScore %1%");
  IMP_SINGLETON_SCORE(MaximumSingletonScore);
};

IMP_OBJECTS(MaximumSingletonScore,MaximumSingletonScores);

IMPCONTAINER_END_NAMESPACE

#endif  /* IMPCONTAINER_MAXIMUM_SINGLETON_SCORE_H */
