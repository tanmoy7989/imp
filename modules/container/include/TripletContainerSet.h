/**
 *  \file IMP/container/TripletContainerSet.h
 *  \brief Store a set of TripletContainers
 *
 *  This file is generated by a script (tools/maintenance/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 */

#ifndef IMPCONTAINER_TRIPLET_CONTAINER_SET_H
#define IMPCONTAINER_TRIPLET_CONTAINER_SET_H

#include "container_config.h"
#include <IMP/TripletContainer.h>
#include <IMP/container_macros.h>
#include <IMP/internal/container_helpers.h>
#include <IMP/scoped.h>

IMPCONTAINER_BEGIN_NAMESPACE

//! Stores a set of TripletContainers
/** The input sets must be disjoint. This can change if there is
    demand for it.

    \usesconstraint
*/
class IMPCONTAINEREXPORT TripletContainerSet
  : public TripletContainer
{
  static TripletContainerSet* get_set(TripletContainer* c) {
    return dynamic_cast<TripletContainerSet*>(c);
  }
 public:
  //! Construct and empty set
  TripletContainerSet(Model *m,
                        std::string name="TripletContainerSet %1%");

  TripletContainerSet(const TripletContainersTemp &pc,
                        std::string name="TripletContainerSet %1%");

  /** \brief apply modifer sm to all triplet containers */
  void apply(const TripletModifier *sm) const;

 /** \brief evaluates all triplet containers using triplet score

     @param[in]   s  triplet score to evaluate each triplet container
     @param[out]  da derivative accumulator when scoring each triplet

     @return the sum of evaluation over all triplet containers
  */
  double evaluate(const TripletScore *s,
                  DerivativeAccumulator *da) const;

 /** \brief evaluates all triplet containers as long as below some maximal
            score threshold

     Evaluates all triplet containers using triplet score s
     until the specified maximal total evaluation score is breached,
     in order to save futile computation time.

     @param[in]   s   triplet score for evaluating each triplet
                      container
     @param[out]  da  derivative accumulator when scoring each triplet
     @param[in]   max the maximal total evaluation score that is allowed

     @return the sum of evaluation at the end of evaluation, or right after
             the maximal value was first breached
  */
  double evaluate_if_good(const TripletScore *s,
                          DerivativeAccumulator *da,
                          double max) const;

  /** \brief apply template derivative modifer sm to all triplet containers

      @param[in]   sm the template derivate modifier to be applied
      @param[out]  da derivative accumulator when applying sm
  */
  template <class SM>
    void template_apply(const SM *sm,
                        DerivativeAccumulator &da) const {
    for (unsigned int i=0; i< get_number_of_triplet_containers(); ++i)
      {
        get_triplet_container(i)->apply(sm, da);
      }
  }

  /** \brief apply template modifer sm to all triplet containers */
  template <class SM>
    void template_apply(const SM *sm) const {
    for (unsigned int i=0; i< get_number_of_triplet_containers(); ++i)
      {
        get_triplet_container(i)->apply(sm);
      }
  }

  /** \brief evaluates all triplet containers using template triplet
             score s

      @param[in]   s  the template for scoring each triplet container
      @param[out]  da derivative accumulator when scoring each triplet

      @return the sum of evaluation over all triplet containers
  */
  template <class SS>
    double template_evaluate(const SS *s,
                             DerivativeAccumulator *da) const {
    double ret=0;
    for (unsigned int i=0; i< get_number_of_triplet_containers(); ++i)
      {
        ret+=get_triplet_container(i)->evaluate(s, da);
      }
    return ret;
  }

 /** \brief evaluates all triplet containers as long as below some maximal
            score threshold

     evaluates all triplet containers using template triplet score s,
     terminates if the specified maximal total evaluation score is breached
     in order to save futile computation time.

     @param[in]   s   the template for scoring each triplet container
     @param[out]  da  derivative accumulator when scoring each triplet
     @param[in]   max the maximal total evaluation score that is allowed

     @return the sum of evaluation at the end of the evaluation, or right after
             the maximum value was first breached
  */
  template <class SS>
    double template_evaluate_if_good(const SS *s,
                                 DerivativeAccumulator *da, double max) const {
    double ret=0;
    for (unsigned int i=0; i< get_number_of_triplet_containers(); ++i)
      {
        double cur=
          get_triplet_container(i)->evaluate_if_good(s, da, max);
        ret+=cur;
        max-=cur;
        if (max < 0) break;
      }
    return ret;
  }

  bool get_is_changed() const;
  ParticleIndexes get_all_possible_indexes() const;
  IMP_OBJECT(TripletContainerSet);

  /** @name Methods to control the nested container

      This container merges a set of nested containers. To add
      or remove nested containers, use the methods below.
  */
  /**@{*/
  IMP_LIST_ACTION(public, TripletContainer, TripletContainers,
                  triplet_container, triplet_containers,
                  TripletContainer*, TripletContainers,
                  {
                    obj->set_was_used(true);
                    set_is_changed(true);
                  },{},
                  );
  /**@}*/
#ifndef IMP_DOXYGEN
  ParticleIndexTriplets get_indexes() const;
  ParticleIndexTriplets get_range_indexes() const;
  ModelObjectsTemp do_get_inputs() const;
  void do_before_evaluate();
#endif
};


IMPCONTAINER_END_NAMESPACE

#endif  /* IMPCONTAINER_TRIPLET_CONTAINER_SET_H */
