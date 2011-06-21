/**
 *  \file TripletContainerSet.cpp
 *  \brief A set of TripletContainers.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 *
 */

#include "IMP/container/TripletContainerSet.h"
#include <algorithm>


IMPCONTAINER_BEGIN_NAMESPACE

namespace {
  TripletContainerSet* get_set(TripletContainer* c) {
    return dynamic_cast<TripletContainerSet*>(c);
  }
}

TripletContainerSet
::TripletContainerSet() {
}

TripletContainerSet
::TripletContainerSet(Model *m, std::string name):
  TripletContainer(m, name),
  deps_(new DependenciesScoreState(this), m){
}

namespace {
  Model *my_get_model(const TripletContainersTemp &in) {
    if (in.empty()) {
      IMP_THROW("Cannot initialize from empty list of containers.",
                IndexException);
    }
    return in[0]->get_model();
  }
}

TripletContainerSet
::TripletContainerSet(const TripletContainersTemp& in,
                        std::string name):
  TripletContainer(my_get_model(in), name),
  deps_(new DependenciesScoreState(this), my_get_model(in)){
  set_triplet_containers(in);
}


bool
TripletContainerSet
::get_contains_particle_triplet(const ParticleTriplet& vt) const {
  for (TripletContainerConstIterator it= triplet_containers_begin();
       it != triplet_containers_end(); ++it) {
    if ((*it)->get_contains_particle_triplet(vt)) return true;
  }
  return false;
}

void TripletContainerSet::do_show(std::ostream &out) const {
  IMP_CHECK_OBJECT(this);
  out << get_number_of_particle_triplets()
      << " containers" << std::endl;
}

unsigned int
TripletContainerSet::get_number_of_particle_triplets() const {
  unsigned int sum=0;
  for (TripletContainerConstIterator it= triplet_containers_begin();
       it != triplet_containers_end(); ++it) {
    sum+= (*it)->get_number_of_particle_triplets();
  }
  return sum;
}

ParticleTriplet
TripletContainerSet::get_particle_triplet(unsigned int i) const {
  for (TripletContainerConstIterator it= triplet_containers_begin();
       it != triplet_containers_end(); ++it) {
    if ( i >= (*it)->get_number_of_particle_triplets()) {
      i-= (*it)->get_number_of_particle_triplets();
    } else {
      return (*it)->get_particle_triplet(i);
    }
  }
  throw IndexException("out of range");
}



IMP_LIST_IMPL(TripletContainerSet,
              TripletContainer,
              triplet_container,
              TripletContainer*,
              TripletContainers,
              {
                if (get_has_added_and_removed_containers()) {
                  get_set(get_added_container())
                    ->add_triplet_container(obj
                           ->get_added_container());
                }
                obj->set_was_used(true);
              },{},
              if (container
                  && container->get_has_added_and_removed_containers()) {
                get_set(container->get_removed_container())
                  ->add_triplet_container(obj
                       ->get_removed_container());
              });


void TripletContainerSet::apply(const TripletModifier *sm) {
  for (unsigned int i=0; i< get_number_of_triplet_containers(); ++i) {
    get_triplet_container(i)->apply(sm);
  }
}

void TripletContainerSet::apply(const TripletModifier *sm,
                               DerivativeAccumulator &da) {
  for (unsigned int i=0; i< get_number_of_triplet_containers(); ++i) {
    get_triplet_container(i)->apply(sm, da);
  }
}

double TripletContainerSet::evaluate(const TripletScore *s,
                                       DerivativeAccumulator *da) const {
  double score=0;
  for (unsigned int i=0; i< get_number_of_triplet_containers(); ++i) {
    score+=get_triplet_container(i)->evaluate(s, da);
  }
  return score;
}


ParticlesTemp TripletContainerSet::get_contained_particles() const {
  ParticlesTemp ret;
  for (unsigned int i=0; i< get_number_of_triplet_containers(); ++i) {
    ParticlesTemp cur= get_triplet_container(i)->get_contained_particles();
    ret.insert(ret.end(), cur.begin(), cur.end());
  }
  return ret;
}



IMPCONTAINER_END_NAMESPACE
