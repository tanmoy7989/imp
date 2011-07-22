/**
 *  \file QuadContainerSet.cpp
 *  \brief A set of QuadContainers.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 *
 */

#include "IMP/container/QuadContainerSet.h"
#include <algorithm>


IMPCONTAINER_BEGIN_NAMESPACE


QuadContainerSet
::QuadContainerSet() {
}

QuadContainerSet
::QuadContainerSet(Model *m, std::string name):
  QuadContainer(m, name),
  deps_(new DependenciesScoreState(this), m){
}

namespace {
  Model *my_get_model(const QuadContainersTemp &in) {
    if (in.empty()) {
      IMP_THROW("Cannot initialize from empty list of containers.",
                IndexException);
    }
    return in[0]->get_model();
  }
}

QuadContainerSet
::QuadContainerSet(const QuadContainersTemp& in,
                        std::string name):
  QuadContainer(my_get_model(in), name),
  deps_(new DependenciesScoreState(this), my_get_model(in)){
  set_quad_containers(in);
}


bool
QuadContainerSet
::get_contains_particle_quad(const ParticleQuad& vt) const {
  for (QuadContainerConstIterator it= quad_containers_begin();
       it != quad_containers_end(); ++it) {
    if ((*it)->get_contains_particle_quad(vt)) return true;
  }
  return false;
}

void QuadContainerSet::do_show(std::ostream &out) const {
  IMP_CHECK_OBJECT(this);
  out << get_number_of_particle_quads()
      << " containers" << std::endl;
}

unsigned int
QuadContainerSet::get_number_of_particle_quads() const {
  unsigned int sum=0;
  for (QuadContainerConstIterator it= quad_containers_begin();
       it != quad_containers_end(); ++it) {
    sum+= (*it)->get_number_of_particle_quads();
  }
  return sum;
}

ParticleQuad
QuadContainerSet::get_particle_quad(unsigned int i) const {
  for (QuadContainerConstIterator it= quad_containers_begin();
       it != quad_containers_end(); ++it) {
    if ( i >= (*it)->get_number_of_particle_quads()) {
      i-= (*it)->get_number_of_particle_quads();
    } else {
      return (*it)->get_particle_quad(i);
    }
  }
  throw IndexException("out of range");
}



IMP_LIST_IMPL(QuadContainerSet,
              QuadContainer,
              quad_container,
              QuadContainer*,
              QuadContainers);


void QuadContainerSet::apply(const QuadModifier *sm) {
  for (unsigned int i=0; i< get_number_of_quad_containers(); ++i) {
    get_quad_container(i)->apply(sm);
  }
}

void QuadContainerSet::apply(const QuadDerivativeModifier *sm,
                               DerivativeAccumulator &da) {
  for (unsigned int i=0; i< get_number_of_quad_containers(); ++i) {
    get_quad_container(i)->apply(sm, da);
  }
}

double QuadContainerSet::evaluate(const QuadScore *s,
                                       DerivativeAccumulator *da) const {
  return template_evaluate(s, da);
}

double QuadContainerSet::evaluate_if_good(const QuadScore *s,
                                               DerivativeAccumulator *da,
                                               double max) const {
  return template_evaluate_if_good(s, da, max);
}


ParticlesTemp QuadContainerSet::get_contained_particles() const {
  ParticlesTemp ret;
  for (unsigned int i=0; i< get_number_of_quad_containers(); ++i) {
    ParticlesTemp cur= get_quad_container(i)->get_contained_particles();
    ret.insert(ret.end(), cur.begin(), cur.end());
  }
  return ret;
}



IMPCONTAINER_END_NAMESPACE
