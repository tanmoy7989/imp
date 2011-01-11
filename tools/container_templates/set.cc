/**
 *  \file CLASSNAMEContainerSet.cpp
 *  \brief A set of CLASSNAMEContainers.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 *
 */

#include "IMP/container/CLASSNAMEContainerSet.h"
#include <algorithm>


IMPCONTAINER_BEGIN_NAMESPACE

namespace {
  CLASSNAMEContainerSet* get_set(CLASSNAMEContainer* c) {
    return dynamic_cast<CLASSNAMEContainerSet*>(c);
  }
}

CLASSNAMEContainerSet
::CLASSNAMEContainerSet() {
}

CLASSNAMEContainerSet
::CLASSNAMEContainerSet(Model *m, std::string name):
  CLASSNAMEContainer(m, name),
  deps_(new DependenciesScoreState(this), m){
}

namespace {
  Model *my_get_model(const CLASSNAMEContainersTemp &in) {
    if (in.empty()) {
      IMP_THROW("Cannot initialize from empty list of containers.",
                IndexException);
    }
    return in[0]->get_model();
  }
}

CLASSNAMEContainerSet
::CLASSNAMEContainerSet(const CLASSNAMEContainersTemp& in,
                        std::string name):
  CLASSNAMEContainer(my_get_model(in), name),
  deps_(new DependenciesScoreState(this), my_get_model(in)){
  set_CLASSFUNCTIONNAME_containers(in);
}


bool
CLASSNAMEContainerSet
::get_contains_FUNCTIONNAME(ARGUMENTTYPE vt) const {
  for (CLASSNAMEContainerConstIterator it= CLASSFUNCTIONNAME_containers_begin();
       it != CLASSFUNCTIONNAME_containers_end(); ++it) {
    if ((*it)->get_contains_FUNCTIONNAME(vt)) return true;
  }
  return false;
}

void CLASSNAMEContainerSet::do_show(std::ostream &out) const {
  IMP_CHECK_OBJECT(this);
  out << get_number_of_FUNCTIONNAMEs()
      << " containers" << std::endl;
}

unsigned int
CLASSNAMEContainerSet::get_number_of_FUNCTIONNAMEs() const {
  unsigned int sum=0;
  for (CLASSNAMEContainerConstIterator it= CLASSFUNCTIONNAME_containers_begin();
       it != CLASSFUNCTIONNAME_containers_end(); ++it) {
    sum+= (*it)->get_number_of_FUNCTIONNAMEs();
  }
  return sum;
}

VARIABLETYPE
CLASSNAMEContainerSet::get_FUNCTIONNAME(unsigned int i) const {
  for (CLASSNAMEContainerConstIterator it= CLASSFUNCTIONNAME_containers_begin();
       it != CLASSFUNCTIONNAME_containers_end(); ++it) {
    if ( i >= (*it)->get_number_of_FUNCTIONNAMEs()) {
      i-= (*it)->get_number_of_FUNCTIONNAMEs();
    } else {
      return (*it)->get_FUNCTIONNAME(i);
    }
  }
  throw IndexException("out of range");
}



IMP_LIST_IMPL(CLASSNAMEContainerSet,
              CLASSNAMEContainer,
              CLASSFUNCTIONNAME_container,
              CLASSNAMEContainer*,
              CLASSNAMEContainers,
              {
                if (get_has_added_and_removed_containers()) {
                  get_set(get_added_container())
                    ->add_CLASSFUNCTIONNAME_container(obj
                           ->get_added_container());
                }
                obj->set_was_used(true);
              },{},
              if (container
                  && container->get_has_added_and_removed_containers()) {
                get_set(container->get_removed_container())
                  ->add_CLASSFUNCTIONNAME_container(obj
                       ->get_removed_container());
              });


void CLASSNAMEContainerSet::apply(const CLASSNAMEModifier *sm) {
  for (unsigned int i=0; i< get_number_of_CLASSFUNCTIONNAME_containers(); ++i) {
    get_CLASSFUNCTIONNAME_container(i)->apply(sm);
  }
}

void CLASSNAMEContainerSet::apply(const CLASSNAMEModifier *sm,
                               DerivativeAccumulator &da) {
  for (unsigned int i=0; i< get_number_of_CLASSFUNCTIONNAME_containers(); ++i) {
    get_CLASSFUNCTIONNAME_container(i)->apply(sm, da);
  }
}

double CLASSNAMEContainerSet::evaluate(const CLASSNAMEScore *s,
                                       DerivativeAccumulator *da) const {
  double score=0;
  for (unsigned int i=0; i< get_number_of_CLASSFUNCTIONNAME_containers(); ++i) {
    score+=get_CLASSFUNCTIONNAME_container(i)->evaluate(s, da);
  }
  return score;
}

double CLASSNAMEContainerSet::evaluate_change(const CLASSNAMEScore *s,
                                              DerivativeAccumulator *da) const {
  double score=0;
  for (unsigned int i=0; i< get_number_of_CLASSFUNCTIONNAME_containers(); ++i) {
    score+=get_CLASSFUNCTIONNAME_container(i)->evaluate_change(s, da);
  }
  return score;
}

double CLASSNAMEContainerSet::evaluate_prechange(const CLASSNAMEScore *s,
                                             DerivativeAccumulator *da) const {
  double score=0;
  for (unsigned int i=0; i< get_number_of_CLASSFUNCTIONNAME_containers(); ++i) {
    score+=get_CLASSFUNCTIONNAME_container(i)->evaluate_prechange(s, da);
  }
  return score;
}


ParticlesTemp CLASSNAMEContainerSet::get_contained_particles() const {
  ParticlesTemp ret;
  for (unsigned int i=0; i< get_number_of_CLASSFUNCTIONNAME_containers(); ++i) {
    ParticlesTemp cur= get_CLASSFUNCTIONNAME_container(i)->get_contained_particles();
    ret.insert(ret.end(), cur.begin(), cur.end());
  }
  return ret;
}

bool CLASSNAMEContainerSet::get_contained_particles_changed() const {
  for (unsigned int i=0; i< get_number_of_CLASSFUNCTIONNAME_containers(); ++i) {
    if (get_CLASSFUNCTIONNAME_container(i)->get_contained_particles_changed()) {
      return true;
    }
  }
  return false;
}


IMPCONTAINER_END_NAMESPACE
