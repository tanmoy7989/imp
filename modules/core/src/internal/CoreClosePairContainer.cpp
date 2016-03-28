/**
 *  \file ClosePairContainer.cpp   \brief A list of ParticlePairs.
 *
 *  Copyright 2007-2016 IMP Inventors. Close rights reserved.
 *
 */

#include "IMP/core/internal/CoreClosePairContainer.h"
#include <IMP/core/BoxSweepClosePairsFinder.h>
#include <IMP/core/GridClosePairsFinder.h>
#include <IMP/core/internal/close_pairs_helpers.h>
#include <IMP/core/pair_predicates.h>
#include <IMP/generic.h>
#include <IMP/PairModifier.h>
#include <boost/unordered_set.hpp>
#include <IMP/utility.h>
#include <algorithm>

#include <IMP/core/RigidClosePairsFinder.h>
#include <IMP/core/rigid_bodies.h>

IMPCORE_BEGIN_INTERNAL_NAMESPACE

IntKey InList::key_ = IntKey("in list temp");

IMP_LIST_IMPL(CoreClosePairContainer, PairFilter, pair_filter, PairFilter *,
              PairFilters);

CoreClosePairContainer::CoreClosePairContainer(SingletonContainer *c,
                                               double distance,
                                               ClosePairsFinder *cpf,
                                               double slack, std::string name)
    : IMP::internal::ListLikeContainer<PairContainer>(c->get_model(),
                                                                 name) {
  initialize(c, distance, slack, cpf);
  // initialize is called from elsewhere, just do this once
  score_state_ = new SS(this);
}

void CoreClosePairContainer::initialize(SingletonContainer *c, double distance,
                                        double slack, ClosePairsFinder *cpf) {
  moved_count_ = 0;
  slack_ = slack;
  distance_ = distance;
  c_ = c;
  cpf_ = cpf;
  cpf_->set_distance(distance_ + 2 * slack_);
  first_call_ = true;
  moved_ = cpf_->get_moved_singleton_container(c_, slack_);
  updates_ = 0;
  rebuilds_ = 0;
  partial_rebuilds_ = 0;
}

void CoreClosePairContainer::set_slack(double s) {
  slack_ = s;
  cpf_->set_distance(distance_ + 2 * slack_);
  ParticleIndexPairs et;
  swap(et);
  first_call_ = true;
}

ModelObjectsTemp CoreClosePairContainer::do_get_inputs() const {
  ModelObjectsTemp ret;
  ret.push_back(c_);
  ret.push_back(score_state_);
  ret.push_back(moved_);
  return ret;
}

ModelObjectsTemp CoreClosePairContainer::get_score_state_inputs()
    const {
  ParticleIndexes all = c_->get_all_possible_indexes();
  ModelObjectsTemp ret = IMP::get_particles(get_model(), all);
  for (unsigned int i = 0; i < get_number_of_pair_filters(); ++i) {
    ret += get_pair_filter(i)->get_inputs(get_model(), all);
  }
  ret += cpf_->get_inputs(get_model(), all);
  ret.push_back(c_);
  ret.push_back(moved_);
  return ret;
}

void CoreClosePairContainer::check_duplicates_input() const {
  ParticlesTemp ps = c_->get_particles();
  std::sort(ps.begin(), ps.end());
  IMP_USAGE_CHECK(std::unique(ps.begin(), ps.end()) == ps.end(),
                  "Duplicates in input");
}

void CoreClosePairContainer::check_list(bool check_slack) const {
  IMP_IF_CHECK(USAGE_AND_INTERNAL) {
    ParticleIndexPairs cur = get_access();
    IMP_INTERNAL_CHECK(
        c_->get_indexes().size() * (c_->get_indexes().size() - 1) / 2 >=
            cur.size(),
        "There are too many particles in the nbl. Currently "
            << cur.size() << " but there can only be "
            << c_->get_indexes().size() * (c_->get_indexes().size() - 1) / 2);
    boost::unordered_set<ParticleIndexPair> existings(cur.begin(),
                                                              cur.end());
    unsigned int num = cur.size();
    IMP_UNUSED(num);
    for (unsigned int j = 0; j < num; ++j) {
      for (unsigned int i = 0; i < get_number_of_pair_filters(); ++i) {
        IMP_INTERNAL_CHECK(
            !get_pair_filter(i)->get_value_index(get_model(), cur[j]),
            "Pair " << cur[j] << " should have been filtered by filter "
                    << get_pair_filter(i)->get_name());
        IMP_INTERNAL_CHECK(!get_pair_filter(i)->get_value_index(
                                get_model(), ParticleIndexPair(
                                                 (cur[j])[0], (cur[j])[1])),
                           "Filter is not symmetric on pair "
                               << cur[j] << get_pair_filter(i)->get_name());
      }
      IMP_INTERNAL_CHECK(cur[j][0] < cur[j][1], "Pair " << cur[j]
                                                        << " is not ordered");
      // removal is lazy, so we can't guarantee this
      /*double d= core::get_distance(XYZR(get_model(), cur[j][0]),
                             XYZR(get_model(), cur[j][1]));
      IMP_INTERNAL_CHECK(d < 4.2*slack_+distance_,
                         "Particles are too far apart: " << cur[j]
                         << " at " << d << " vs " << 4.0*slack_+distance_);*/
    }
    IMP_INTERNAL_CHECK(
        existings.size() == num,
        "Not all particle pairs in list are unique: "
            << num << " vs " << existings.size() << " lists " << get_access()
            << " vs " << ParticleIndexPairs(
                             existings.begin(), existings.end()) << std::endl);
    double check_distance = distance_ * .9;
    if (check_slack) {
      check_distance += 1.8 * slack_;
    }
    cpf_->set_distance(check_distance);
    cpf_->set_pair_filters(access_pair_filters());
    ParticleIndexPairs found = cpf_->get_close_pairs(get_model(),
                                                             c_->get_indexes());
    internal::filter_close_pairs(this, found);
    IMP_LOG_TERSE("In check found " << found << std::endl);
    for (unsigned int i = 0; i < found.size(); ++i) {
      ParticleIndexPair pi(found[i][0], found[i][1]);
      ParticleIndexPair pii(found[i][1], found[i][0]);
      IMP_INTERNAL_CHECK(
          existings.find(pi) != existings.end() ||
              existings.find(pii) != existings.end(),
          "Pair " << pi << " not found in close pairs list"
                  << " at distance "
                  << core::get_distance(XYZR(get_model(), found[i][0]),
                                        XYZR(get_model(), found[i][1])));
    }
  }
}

void CoreClosePairContainer::do_first_call() {
  IMP_LOG_TERSE("Handling first call of ClosePairContainer." << std::endl);
  IMP_IF_CHECK(USAGE_AND_INTERNAL) { check_duplicates_input(); }
  do_rebuild();
  first_call_ = false;
}

void CoreClosePairContainer::do_incremental() {
  IMP_LOG_TERSE("Handling incremental update of ClosePairContainer"
                << std::endl);
  ++partial_rebuilds_;
  using IMP::operator<<;
  IMP_LOG_VERBOSE("Moved " << moved_->get_indexes() << std::endl);
  PairPredicatesTemp pf = access_pair_filters();
  pf.push_back(new AllSamePairPredicate());
  pf.back()->set_was_used(true);
  cpf_->set_pair_filters(pf);
  cpf_->set_distance(distance_ + 2 * slack_);
  ParticleIndexPairs ret;
  IMP_CONTAINER_ACCESS(SingletonContainer, moved_, {
    const ParticleIndexes &moved = imp_indexes;
    IMP_CONTAINER_ACCESS(
        SingletonContainer, c_,
        ret = cpf_->get_close_pairs(get_model(), imp_indexes, moved));
    ParticleIndexPairs ret1 = cpf_->get_close_pairs(get_model(), moved);
    ret.insert(ret.begin(), ret1.begin(), ret1.end());
    internal::fix_order(ret);
    moved_count_ += moved.size();
  });
  {
    /*InList il= InList::create(moved);
      remove_from_list_if(il);
      InList::destroy(il);*/
    ParticleIndexPairs cur;
    swap(cur);
    cur.erase(std::remove_if(cur.begin(), cur.end(),
                             FarParticle(get_model(), distance_ + 2 * slack_)),
              cur.end());
    swap(cur);
    moved_count_ = 0;
  }
  IMP_LOG_TERSE("Found " << ret.size() << " pairs." << std::endl);
  {
    // now insert
    std::sort(ret.begin(), ret.end());
    ParticleIndexPairs all;
    swap(all);
    unsigned int osz = all.size();
    all.insert(all.end(), ret.begin(), ret.end());
    std::inplace_merge(all.begin(), all.begin() + osz, all.end());
    all.erase(std::unique(all.begin(), all.end()), all.end());
    swap(all);
  }
  moved_->reset_moved();
  IMP_LOG_TERSE("Count is now " << get_access().size() << std::endl);
}

void CoreClosePairContainer::do_rebuild() {
  IMP_LOG_TERSE("Handling full update of ClosePairContainer." << std::endl);
  ++rebuilds_;
  cpf_->set_pair_filters(access_pair_filters());
  cpf_->set_distance(distance_ + 2 * slack_);
  ParticleIndexPairs ret =
      cpf_->get_close_pairs(get_model(), c_->get_indexes());
  internal::fix_order(ret);
  IMP_LOG_TERSE("Found before filtering " << ret << " pairs." << std::endl);
  internal::filter_close_pairs(this, ret);
  IMP_LOG_TERSE("Found " << ret << " pairs." << std::endl);
  std::sort(ret.begin(), ret.end());
  swap(ret);
  moved_->reset();
}

void CoreClosePairContainer::do_score_state_before_evaluate() {
  IMP_OBJECT_LOG;
  IMP_CHECK_OBJECT(c_);
  IMP_CHECK_OBJECT(cpf_);
  set_was_used(true);
  ++updates_;
  try {
    IMP_LOG_TERSE("Moved count is " << moved_->get_access().size()
                                    << std::endl);
    if (first_call_) {
      do_first_call();
      check_list(true);
    } else if (moved_->get_access().size() != 0) {
      if (moved_->get_access().size() < 1000) {
        do_incremental();
        check_list(false);
      } else {
        do_rebuild();
        check_list(true);
      }
    } else {
      IMP_LOG_TERSE("No particles moved more than " << slack_ << std::endl);
      check_list(false);
    }
  }
  catch (std::bad_alloc &) {
    IMP_THROW("Ran out of memory when computing close pairs."
                  << " Try to reduce the "
                  << "slack or reformulate the problem.",
              ValueException);
  }
}

ParticleIndexPairs CoreClosePairContainer::get_range_indexes() const {
  ParticleIndexes pis = c_->get_range_indexes();
  ParticleIndexPairs ret;
  ret.reserve(pis.size() * (pis.size() - 1) / 2);
  for (unsigned int i = 0; i < pis.size(); ++i) {
    for (unsigned int j = 0; j < i; ++j) {
      ret.push_back(ParticleIndexPair(pis[i], pis[j]));
    }
  }
  internal::filter_close_pairs(this, ret);
  return ret;
}

ParticleIndexes CoreClosePairContainer::get_all_possible_indexes() const {
  ParticleIndexes ret = c_->get_all_possible_indexes();
  return ret;
}

IMPCORE_END_INTERNAL_NAMESPACE
