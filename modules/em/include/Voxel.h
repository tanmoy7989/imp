/**
 *  \file Voxel.h     \brief voxel decorator.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPEM_VOXEL_H
#define IMPEM_VOXEL_H

#include "em_config.h"
#include <IMP/Decorator.h>
#include <IMP/core/XYZ.h>
#include <IMP/core/XYZR.h>
#include <IMP/algebra/Vector3D.h>
#include <IMP/algebra/Sphere3D.h>

IMPEM_BEGIN_NAMESPACE

//! A a decorator for a particle that stores data on a density voxel , such as:
//! position (x,y,z) attributes and density attributes
/**
    \ingroup decorators
 */
class IMPEMEXPORT Voxel: public Decorator
{
 public:

  static const FloatKey get_density_key();
  static const FloatKeys get_keys();

  IMP_DECORATOR(Voxel, Decorator);

  /** Create a decorator with the passed coordinates. */
  static Voxel setup_particle(Particle *p,
                  const algebra::VectorD<3> &position,
                  Float radius,
                  Float density) {
    core::XYZR::setup_particle(p,algebra::SphereD<3>(position,radius));
    p->add_attribute(get_density_key(),density,false);
    return Voxel(p);
  }

  IMP_DECORATOR_GET_SET(density, get_density_key(), Float, Float);

  static bool particle_is_instance(Particle *p) {
    core::XYZ::particle_is_instance(p);
    IMP_USAGE_CHECK( p->has_attribute(get_density_key()),
              "Particle is expected to have density attribute.");
    return true;
  }
};

IMP_OUTPUT_OPERATOR(Voxel);

typedef Decorators<Voxel, Particles> Voxels;

IMPEM_END_NAMESPACE

#endif  /* IMPEM_VOXEL_H */
