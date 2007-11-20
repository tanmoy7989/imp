/**
 *  \file decorators/DecoratorBase.h    \brief The base class for decorators.
 *
 *  Copyright 2007 Sali Lab. All rights reserved.
 *
 */

#ifndef __IMP_DECORATOR_BASE_H
#define __IMP_DECORATOR_BASE_H

namespace IMP
{

/** A base class for decorators. Implementers of decorators should
    just inherit from this and then use the IMP_DECORATOR macro
*/
class IMPDLLEXPORT DecoratorBase
{
protected:
  Particle *particle_;
  DecoratorBase(Particle *p): particle_(p) {}
  bool is_default() const {
    return particle_==NULL;
  }
  static bool has_required_attributes(Particle *p) {
    return true;
  }
  static void add_required_attributes(Particle *p) {
  }
  static void decorator_initialize_static_data() {}
public:
  typedef DecoratorBase This;

  DecoratorBase():particle_(NULL) {}
  IMP_COMPARISONS_1(particle_);

  /** \return the particle wrapped by this decorator*/
  Particle *get_particle() const {
    return particle_;
  }
  /** \return the Model containing the particle */
  Model *get_model() const {
    return particle_->get_model();
  }

  //! create a decorator of class D from the Particle
  /**
     This is mostly for use through the D::cast function
   */
  template <class D>
  static D cast(Particle *p) {
    D:: decorator_initialize_static_data();
    if (!D::has_required_attributes(p)) return D();
    else return D(p);
  }

  //! create a decorator of class D from the Particle
  /**
     This is mostly for use through the D::create function
   */
  template <class D>
  static D create(Particle *p) {
    D::decorator_initialize_static_data();
    D::add_required_attributes(p);
    return D(p);
  }

};

}

#endif
