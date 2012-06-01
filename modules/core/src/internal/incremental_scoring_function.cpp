/**
 *  \file MonteCarlo.cpp  \brief Simple Monte Carlo optimizer.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 *
 */

#include <IMP/core/internal/incremental_scoring_function.h>
#include <IMP/core/internal/SingleParticleScoringFunction.h>
#include <IMP/internal/InternalListSingletonContainer.h>
#include <IMP/core/internal/CoreClosePairContainer.h>
#include <IMP/core/GridClosePairsFinder.h>
#include <IMP/internal/InternalPairsRestraint.h>
#include <IMP/core/internal/close_pairs_helpers.h>
#include <IMP/core/XYZ.h>
#include <IMP/core/XYZR.h>
#include <numeric>
IMPCORE_BEGIN_INTERNAL_NAMESPACE
/** to handle good/max evaluate, add dummy restraints for each
    restraint set that return 0 or inf if the last scores for the
    set are bad.*/


NBLScoring::NBLScoring(PairScore *ps,
                       double distance,
                       const ParticleIndexes &to_move,
                       const ParticlesTemp &particles,
                       const PairPredicates &filters,
                       double weight, double max):
  cache_(IMP::internal::get_index(particles),
         NBGenerator(IMP::internal::get_model(particles),
                     IMP::internal::get_index(particles),
                     ps,
                     distance, filters),
         NBChecker(IMP::internal::get_model(particles),
                   IMP::internal::get_index(particles),
                   ps,
                   distance, filters)) {
  weight_=weight;
  max_=max;
  to_move_= to_move;
  DependencyGraph dg= get_dependency_graph(particles[0]->get_model());
  DependencyGraphVertexIndex dgi= get_vertex_index(dg);
  update_dependencies(dg, dgi);
  dummy_restraint_= create_restraint();
}

void
NBLScoring::update_dependencies( const DependencyGraph& dg,
                                 const DependencyGraphVertexIndex &index) {
  for (unsigned int i=0; i< to_move_.size(); ++i) {
    Particle *p= cache_.get_generator().m_->get_particle(to_move_[i]);
    ParticlesTemp ps= get_dependent_particles(p, ParticlesTemp(),
                                              dg, index);
    // intersect that with the ones I care about
    ParticleIndexes pis= IMP::internal::get_index(ps);
    std::sort(pis.begin(), pis.end());
    ParticleIndexes deps;
    std::set_intersection(pis.begin(), pis.end(),
                          cache_.get_generator().pis_.begin(),
                          cache_.get_generator().pis_.end(),
                          std::back_inserter(deps));
    controlled_[to_move_[i]]=deps;
    IMP_LOG(TERSE, "Particle " << Showable(p) << " controls "
            << IMP::internal::get_particle(cache_.get_generator().m_,
                                           deps) << std::endl);
  }
}
namespace {
struct NBSum {
  double value;
  NBSum(): value(0){}
  template <class T>
  void operator()(const T &t) {
    IMP_LOG(TERSE, "adding " << t << std::endl);
    value+=t.score;
  }
};
struct NBShow {
  template <class T>
  void operator()(const T &t) {
    IMP_LOG(TERSE, t << std::endl);
  }
};
}

void NBLScoring::set_moved(const ParticleIndexes& moved) {
  IMP_FUNCTION_LOG;

  for (unsigned int i=0; i< moved.size(); ++i) {
    ParticleIndexes c=controlled_.find(moved[i])->second;
      IMP_LOG(TERSE, "Got input particle " << moved[i] << " that controls "
              << c << std::endl);
      for (unsigned int i=0; i< c.size(); ++i) {
        cache_.remove(c[i]);
      }
  }
  IMP_LOG(TERSE, "Cleared state is ");
  // must not do apply so we don't fill it up again
  cache_.apply_to_current_contents(NBShow());
  IMP_LOG(TERSE, std::endl);
}

double NBLScoring::get_score() {
  IMP_FUNCTION_LOG;
  return weight_*cache_.apply(NBSum()).value;
}

Restraint* NBLScoring::create_restraint() const {
  IMP_NEW(IMP::internal::InternalListSingletonContainer,
          lsc, (cache_.get_generator().m_, "NBLInput Container %1%"));
  lsc->set_particles(cache_.get_generator().pis_);
  IMP_NEW(IMP::core::internal::CoreClosePairContainer,
          cpc, (lsc, cache_.get_generator().distance_,
                default_cpf(1000),
                cache_.get_generator().distance_*.2));

  Pointer<Restraint> ret= new IMP::internal
    ::InternalPairsRestraint(cache_.get_generator().score_.get(),
                       cpc.get());
  ret->set_model(cache_.get_generator().m_);
  return ret.release();
}


NBGenerator::NBGenerator(Model*m, const ParticleIndexes& pis,
                         PairScore *ps,
                         double distance,
                         const PairPredicates &pfs) {
  m_=m;
  score_=ps;
  pis_=pis;
  std::sort(pis_.begin(), pis_.end());
  filters_=pfs;
  double maxr=0;
  for (unsigned int i=0; i< pis_.size(); ++i) {
    maxr=std::max(maxr, core::XYZR(m, pis_[i]).get_radius());
  }
  distance_=distance+2*maxr;
  filters_=pfs;

  algebra::Vector3Ds vs(pis_.size());
  for (unsigned int i=0; i< pis_.size(); ++i) {
    vs[i]= XYZ(m_, pis_[i]).get_coordinates();
    to_dnn_[pis_[i]]=i;
  }
  dnn_= new algebra::DynamicNearestNeighbor3D(vs, distance_);
  dnn_->set_log_level(IMP::base::SILENT);
}

NBGenerator::result_type
NBGenerator::operator()( argument_type a) const {
  std::sort(a.begin(), a.end());
  //a.erase(std::unique(a.begin(), a.end()), a.end());
  //IMP_LOG(TERSE, "Input of " << ia << " resolves to " << a << std::endl);
  IMP_LOG(TERSE, "Generating pair scores from " << a << std::endl);
  result_type ret;
  for (unsigned  int i=0; i< a.size(); ++i) {
    int di=to_dnn_.find(a[i])->second;
    XYZ d(m_, a[i]);
    dnn_->set_coordinates(di, d.get_coordinates());
  }
  for (unsigned  int i=0; i< a.size(); ++i) {
    int di=to_dnn_.find(a[i])->second;
    Ints n= dnn_->get_in_ball(di,
                              distance_);
    IMP_LOG(TERSE, "Neighbors are " << n << std::endl);
    for (unsigned int j=0; j< n.size(); ++j) {
      // if the partner is not in the list or is a lower index
      ParticleIndex ppi= pis_[n[j]];
      IMP_LOG(VERBOSE, "Checking out pair " << a[i] << " " << ppi << std::endl);
      if (std::find(a.begin(), a.end(), ppi) == a.end()
          || ppi < a[i]) {
        ParticleIndexPair pp(a[i], ppi);
        bool filtered=false;
        for (unsigned int k=0; k< filters_.size(); ++k) {
          if (filters_[k]->get_value_index(m_, pp)) {
            filtered=true;
            break;
          }
        }
        if (!filtered) {
          double score= score_->evaluate_index(m_, pp, nullptr);
          if (score != 0) {
            ret.push_back(single_result_type(a[i], ppi, score));
            IMP_LOG(VERBOSE, "Score for " << pp << " is "
                    << score << std::endl);
          } else {
            IMP_LOG(VERBOSE, "scoreless" <<  std::endl);
          }
        } else {
          IMP_LOG(VERBOSE, "filtered" <<  std::endl);
        }
      } else {
        IMP_LOG(VERBOSE, "redundant" <<  std::endl);
      }
    }
  }
  return ret;
}


NBChecker::NBChecker(Model *m, const ParticleIndexes &pis,
                     PairScore *score, double d,
                     const PairPredicates &filt): m_(m),
                                                  pis_(pis), score_(score),
                                               distance_(d),
                                    filt_(filt) {}
bool NBChecker::
operator()(const NBGenerator::result_type &vals) const {
  IMP_NEW(GridClosePairsFinder, gcpf, ());
  gcpf->set_distance(.9*distance_);
  ParticleIndexPairs found= gcpf->get_close_pairs(m_,
                                                  pis_);
  for (unsigned int i=0; i< filt_.size(); ++i) {
    filt_[i]->remove_if_equal(m_,
                              found, 1);
  }
  compatibility::set<ParticleIndexPair> vals_index;
  for (unsigned int i=0; i< vals.size(); ++i) {
    vals_index.insert(ParticleIndexPair(vals[i].first,
                                        vals[i].second));
  }
  for (unsigned int i=0; i< found.size(); ++i) {
    double score= score_->evaluate_index(m_, found[i], nullptr);
    if (score ==0) continue;
    bool has= vals_index.find(found[i])!= vals_index.end()
      || vals_index.find(ParticleIndexPair(found[i][1], found[i][0]))
      != vals_index.end();
    if (!has) {
      IMP_LOG(SILENT, "Can't find pair " << found[i]
              << " in list " << vals
              << " at distance " << get_distance(XYZR(m_, found[i][0]),
                                                 XYZR(m_, found[i][1]))
              << " with threshold " << distance_
              << std::endl);
      return false;
    }
  }
  for (unsigned int i=0; i< vals.size(); ++i) {
    ParticleIndexPair pip(vals[i].first, vals[i].second);
    double nscore= score_->evaluate_index(m_, pip, nullptr);
    if (std::abs(nscore-vals[i].score) > .1) {
      IMP_LOG(SILENT, "Scores don't match for " << pip << " had "
              << vals[i].score << " and got " << nscore << std::endl);
      return false;
    }
  }
  return true;
}

IMPCORE_END_INTERNAL_NAMESPACE
