/**
 *  \file ListSingletonContainer.h    \brief Store a list of ParticlesTemp
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 */

#ifndef IMPCORE_INTERNAL_CORE_LIST_SINGLETON_CONTAINER_H
#define IMPCORE_INTERNAL_CORE_LIST_SINGLETON_CONTAINER_H

#include "../core_config.h"
#include <IMP/SingletonContainer.h>
#include <IMP/internal/container_helpers.h>
#include "singleton_helpers.h"
#include <IMP/ScoreState.h>

IMPCORE_BEGIN_INTERNAL_NAMESPACE


class IMPCOREEXPORT CoreListSingletonContainer:
public internal::ListLikeSingletonContainer
{
  IMP_ACTIVE_CONTAINER_DECL(CoreListSingletonContainer);
public:
  CoreListSingletonContainer(Model *m, std::string name);
  CoreListSingletonContainer(Model *m, const char *name);
  CoreListSingletonContainer();
  void add_particle(Particle* vt);
  void add_particles(const ParticlesTemp &c);
  void remove_particles(const ParticlesTemp &c);
  void set_particles(ParticlesTemp c);
  void clear_particles();
  bool get_is_up_to_date() const {
    return true;
  }
  IMP_LISTLIKE_SINGLETON_CONTAINER(CoreListSingletonContainer);
};


IMPCORE_END_INTERNAL_NAMESPACE

#endif  /* IMPCORE_INTERNAL_CORE_LIST_SINGLETON_CONTAINER_H */
