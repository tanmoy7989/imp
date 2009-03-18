/**
 *  \file derivative_geometry.h
 *  \brief Display the derivatives of various things
 *
 *  Copyright 2007-9 Sali Lab. All rights reserved.
 */

#ifndef IMPDISPLAY_DERIVATIVE_GEOMETRY_H
#define IMPDISPLAY_DERIVATIVE_GEOMETRY_H

#include "config.h"
#include "macros.h"

#include "internal/version_info.h"
#include <IMP/PairContainer.h>
#include <IMP/core/XYZRDecorator.h>
#include <IMP/core/rigid_bodies.h>
#include <IMP/display/geometry.h>

IMPDISPLAY_BEGIN_NAMESPACE

//! Display the derivatives of an XYZ particle
/** The name is the Particle::get_name() name.
 */
class IMPDISPLAYEXPORT XYZDerivativeGeometry: public Geometry
{
  core::XYZDecorator d_;
  Float radius_;
public:
  //! Get the individual particles from the passed SingletonContainer
  XYZDerivativeGeometry(core::XYZDecorator d, Float radius=0);

  virtual ~XYZDerivativeGeometry();

  virtual Float get_size() const;

  std::string get_name() const {
    return d_.get_particle()->get_name();
  }
  IMP_GEOMETRY(internal::version_info)
};


//! Display the derivatives of an RigidBody particle
/** The name is the Particle::get_name() name.
 */
class IMPDISPLAYEXPORT RigidBodyDerivativeGeometry:
  public CompoundGeometry
{
  Color xyzcolor_, qcolor_, ccolor_;
  core::RigidBodyDecorator d_;
public:
  //! Get the individual particles from the passed SingletonContainer
  RigidBodyDerivativeGeometry(core::RigidBodyDecorator d);

  virtual ~RigidBodyDerivativeGeometry();

  //! Set the color used to display the translational part
  void set_translational_color(Color c) {
    xyzcolor_=c;
  }

  //! Set the color used to display the rotational part
  void set_rotational_color(Color c) {
    qcolor_=c;
  }

  IMP_COMPOUND_GEOMETRY(internal::version_info);
};


IMPDISPLAY_END_NAMESPACE

#endif  /* IMPDISPLAY_DERIVATIVE_GEOMETRY_H */
