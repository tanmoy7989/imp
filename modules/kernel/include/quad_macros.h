/**
 *  \file quad_macros.h    \brief Macros for various classes.
 *
 *  This file is generated by a script (core/tools/make-containers).
 *  Do not edit directly.
 *
 *  Copyright 2007-2012 IMP Inventors. All rights reserved.
 */

#ifndef IMPKERNEL_QUAD_MACROS_H
#define IMPKERNEL_QUAD_MACROS_H

#include "internal/TupleRestraint.h"
#include "internal/functors.h"
#define IMP_QUAD_SCORE_BASE(Name)                                 \
  IMP_IMPLEMENT_INLINE(double evaluate(const ParticleQuadsTemp &ps,    \
                                       DerivativeAccumulator *da) const, { \
    double ret=0;                                                       \
    for (unsigned int i=0; i< ps.size(); ++i) {                         \
      ret+=Name::evaluate(ps[i], da);                                   \
    }                                                                   \
    return ret;                                                         \
                       });                                              \
  IMP_IMPLEMENT_INLINE(double evaluate_indexes(Model *m,                \
                                              const ParticleIndexQuads &ps, \
                                              DerivativeAccumulator *da)\
    const, {                                                            \
    double ret=0;                                                       \
    for (unsigned int i=0; i< ps.size(); ++i) {                         \
      ret+=Name::evaluate_index(m, ps[i], da);                          \
    }                                                                   \
    return ret;                                                         \
                       });                                              \
  IMP_IMPLEMENT_INLINE(double evaluate_if_good_indexes(Model *m, \
                                     const ParticleIndexQuads &ps, \
                          DerivativeAccumulator *da,                    \
                                                      double max) const, { \
    double ret=0;                                                       \
    for (unsigned int i=0; i< ps.size(); ++i) {                         \
      double cur=Name::evaluate_if_good_index(m, ps[i], da, max);       \
      max-=cur;                                                         \
      ret+=cur;                                                         \
      if (max <0) break;                                                \
    }                                                                   \
    return ret;                                                         \
                      });                                               \
  IMP_OBJECT(Name)



//! Declare the functions needed for a QuadScore
/** In addition to the methods done by IMP_INTERACTON, it declares
    - IMP::QuadScore::evaluate(IMP::Particle*,
    IMP::DerivativeAccumulator*)
    - IMP::QuadScore::get_input_particles()
    - IMP::QuadScore::get_output_particles()

    See IMP_SIMPLE_QUAD_SCORE() for a way of providing an
    implementation of that method.
*/
#define IMP_QUAD_SCORE(Name)                                      \
  IMP_IMPLEMENT(double evaluate(const ParticleQuad& p,\
                                DerivativeAccumulator *da) const);      \
  IMP_IMPLEMENT_INLINE(double evaluate_index(Model *m,                  \
                                const ParticleIndexQuad& p,           \
                                     DerivativeAccumulator *da) const, { \
    return evaluate(IMP::internal::get_particle(m,p), da);              \
                                    });                                 \
  IMP_IMPLEMENT_INLINE(double evaluate_if_good_index(Model *m,          \
                          const ParticleIndexQuad& p,                       \
                          DerivativeAccumulator *da,                    \
                                                     double max) const, { \
    IMP_UNUSED(max);                                                    \
    return evaluate_index(m, p, da);                                    \
                       });                                              \
  IMP_IMPLEMENT(ParticlesTemp get_input_particles(Particle*p) const) ;  \
  IMP_IMPLEMENT(ContainersTemp get_input_containers(Particle *) const) ; \
  IMP_QUAD_SCORE_BASE(Name)

//! Declare the functions needed for a QuadScore
/** In addition to the methods declared and defined by IMP_QUAD_SCORE,
    the macro provides an implementation of
    - IMP::QuadScore::get_input_particles()
    - IMP::QuadScore::get_input_containers()
    which assume that only the passed particle serves as input to the
    score.
*/
#define IMP_SIMPLE_QUAD_SCORE(Name)                               \
  IMP_IMPLEMENT(double evaluate(const ParticleQuad& p,    \
                                DerivativeAccumulator *da) const);      \
  IMP_IMPLEMENT_INLINE(double evaluate(Model *m,                        \
                                  const ParticleIndexQuad& p,  \
                                       DerivativeAccumulator *da) const, { \
    return evaluate(IMP::internal::get_particle(m,p), da);              \
                       });                                              \
  IMP_IMPLEMENT_INLINE(double evaluate_if_good_index(Model *m,          \
                          const ParticleIndexQuad& p,                       \
                          DerivativeAccumulator *da,                    \
                                                     double max) const, { \
    IMP_UNUSED(max);                                                    \
    return evaluate_index(m, p, da);                                    \
                       });                                              \
  IMP_IMPLEMENT_INLINE(ParticlesTemp get_input_particles(Particle*p) const, { \
    return ParticlesTemp(1,p);                                          \
    });                                                                 \
  IMP_IMPLEMENT_INLINE(ContainersTemp get_input_containers(Particle *) const, \
  {                                                                     \
    return ContainersTemp();                                            \
  });                                                                   \
  IMP_IMPLEMENT_INLINE(Restraints create_current_decomposition\
  (const ParticleQuad& vt) const, {                                      \
   return Restraints(1, IMP::internal::create_restraint(this, vt)); \
                       });                                        \
  IMP_QUAD_SCORE_BASE(Name)



//! Declare the functions needed for a complex QuadScore
/** In addition to the methods done by IMP_OBJECT(), it declares
    - IMP::QuadScore::evaluate()
    - IMP::QuadScore::get_input_particles()
    - IMP::QuadScore::get_output_particles()
    - IMP::QuadScore::evaluate_if_good
*/
#define IMP_COMPOSITE_QUAD_SCORE(Name)                            \
  IMP_IMPLEMENT(ParticlesTemp get_input_particles(Particle *p) const);  \
  IMP_IMPLEMENT(ContainersTemp get_input_containers(Particle *p) const); \
  IMP_IMPLEMENT_INLINE(double evaluate(const ParticleQuad& p,     \
                                       DerivativeAccumulator *da) const, { \
    return evaluate_index(IMP::internal::get_model(p),                  \
                  IMP::internal::get_index(p), da);                     \
                       });                                              \
  IMP_IMPLEMENT(double evaluate_index(Model *m, const ParticleIndexQuad& p,\
                                      DerivativeAccumulator *da) const); \
  IMP_IMPLEMENT(double evaluate_if_good_index(Model *m,                 \
                          const ParticleIndexQuad& p,                       \
                          DerivativeAccumulator *da,                    \
                                              double max) const);       \
  IMP_QUAD_SCORE_BASE(Name)

//! Declare the functions needed for a complex QuadScore
/** In addition to the methods done by IMP_OBJECT(), it declares
    - IMP::QuadScore::evaluate()
    - IMP::QuadScore::get_input_particles()
    - IMP::QuadScore::get_output_particles()
    - IMP::QuadScore::evaluate_if_good
*/
#define IMP_INDEX_QUAD_SCORE(Name)                                \
  IMP_IMPLEMENT(ParticlesTemp get_input_particles(Particle *p) const);  \
  IMP_IMPLEMENT(ContainersTemp get_input_containers(Particle *p) const); \
  IMP_IMPLEMENT_INLINE(double evaluate(const ParticleQuad& p,\
                                        DerivativeAccumulator *da) const, { \
    return evaluate_index(IMP::internal::get_model(p),                  \
                  IMP::internal::get_index(p),                          \
                  da);                                                  \
                        });                                             \
  IMP_IMPLEMENT(double evaluate_index(Model *m, const ParticleIndexQuad& p,\
                                      DerivativeAccumulator *da) const); \
  IMP_IMPLEMENT_INLINE(double evaluate_if_good_index(Model *m,         \
                          const ParticleIndexQuad& p,                      \
                          DerivativeAccumulator *da,                    \
                                                      double max) const, { \
    IMP_UNUSED(max);                                                    \
    return evaluate_index(m, p, da);                                    \
                        });                                             \
  IMP_QUAD_SCORE_BASE(Name)



//! Declare the functions needed for a QuadPredicate
/** In addition to the methods done by IMP_OBJECT, it declares
    - IMP::QuadPredicate::get_value()
    - IMP::QuadPredicate::get_input_particles()
    - IMP::QuadPredicate::get_output_particles()
*/
#define IMP_QUAD_PREDICATE(Name)                                   \
  IMP_IMPLEMENT(int get_value(const ParticleQuad& a) const);   \
  IMP_IMPLEMENT_INLINE(Ints get_value(const                             \
                              ParticleQuadsTemp &o) const, {   \
    Ints ret(o.size());                                                 \
    for (unsigned int i=0; i< o.size(); ++i) {                          \
      ret[i]+= Name::get_value(o[i]);                                   \
    }                                                                   \
    return ret;                                                         \
    });                                                                 \
  IMP_IMPLEMENT_INLINE(int get_value_index(Model *m,                    \
                                           const ParticleIndexQuad& vt)\
                       const, {                                         \
        return Name::get_value(IMP::internal::get_particle(m, vt)); \
                       });                                              \
  IMP_IMPLEMENT_INLINE(Ints get_value_index(Model *m,                   \
                                     const ParticleIndexQuads &o) const, { \
   Ints ret(o.size());                                                  \
   for (unsigned int i=0; i< o.size(); ++i) {                           \
     ret[i]+= Name::get_value_index(m, o[i]);                           \
   }                                                                    \
   return ret;                                                          \
                       });                                              \
  IMP_IMPLEMENT(ParticlesTemp get_input_particles(Particle*) const);    \
  IMP_IMPLEMENT(ContainersTemp get_input_containers(Particle*) const);  \
  IMP_OBJECT(Name)


//! Declare the functions needed for a QuadPredicate
/** In addition to the methods done by IMP_OBJECT, it declares
    - IMP::QuadPredicate::get_value_index()
    - IMP::QuadPredicate::get_input_particles()
    - IMP::QuadPredicate::get_output_particles()
*/
#define IMP_INDEX_QUAD_PREDICATE(Name, gv)                        \
  IMP_IMPLEMENT_INLINE(int get_value(const ParticleQuad& a) const, { \
    return get_value_index(IMP::internal::get_model(a),                 \
                     IMP::internal::get_index(a));                      \
    });                                                                 \
  IMP_IMPLEMENT_INLINE(Ints get_value(const                             \
                                      ParticleQuadsTemp &o) const, {   \
    Ints ret(o.size());                                                 \
    for (unsigned int i=0; i< o.size(); ++i) {                          \
      ret[i]+= Name::get_value(o[i]);                                   \
    }                                                                   \
    return ret;                                                         \
                       })                                               \
  IMP_IMPLEMENT_INLINE(int get_value_index(Model *m,                    \
                                           const ParticleIndexQuad& pi)\
                       const, {                                         \
    gv;                                                                 \
                       })                                               \
  IMP_IMPLEMENT_INLINE(Ints get_value_index(Model *m,                   \
                                const ParticleIndexQuads &o) const, { \
   Ints ret(o.size());                                                  \
   for (unsigned int i=0; i< o.size(); ++i) {                           \
     ret[i]+= Name::get_value_index(m, o[i]);                           \
   }                                                                    \
   return ret;                                                          \
                       });                                              \
  IMP_IMPLEMENT_INLINE_NO_SWIG(void remove_if_equal(Model *m,           \
                                            ParticleIndexQuads& ps,        \
                                            int value) const, {         \
      ps.erase(std::remove_if(ps.begin(), ps.end(),                     \
                              IMP::internal::PredicateEquals<Name, true>(this, \
                                                              m, value)), \
               ps.end());                                               \
                       });                                              \
  IMP_IMPLEMENT_INLINE_NO_SWIG(void remove_if_not_equal(Model *m,       \
                                            ParticleIndexQuads& ps,        \
                                            int value) const, {         \
      ps.erase(std::remove_if(ps.begin(), ps.end(),                     \
                          IMP::internal::PredicateEquals<Name, false>(this, \
                                                                 m, value)), \
               ps.end());                                               \
                       });                                              \
  IMP_IMPLEMENT_INLINE(ParticlesTemp get_input_particles(Particle*p) const, { \
   return ParticlesTemp(1, p);                                          \
    });                                                                 \
  IMP_IMPLEMENT_INLINE(ContainersTemp get_input_containers(Particle*) const, { \
   return ContainersTemp();                                             \
    });                                                                 \
 IMP_OBJECT_INLINE(Name,IMP_UNUSED(out),)


//! Declare the functions needed for a QuadModifier
/** In addition to the methods done by IMP_OBJECT, it declares
    - IMP::QuadModifier::apply(IMP::Particle*)
    - IMP::QuadModifier::get_input_particles()
    - IMP::QuadModifier::get_output_particles()
*/
#define IMP_QUAD_MODIFIER(Name)                                   \
  IMP_IMPLEMENT(void apply(const ParticleQuad& a) const); \
  IMP_IMPLEMENT_INLINE(void apply_index(Model *m, \
                                        const ParticleIndexQuad& a) const, {\
    return Name::apply(IMP::internal::get_particle(m,a));               \
    })                                                                  \
  IMP_IMPLEMENT(ParticlesTemp get_input_particles(Particle*) const);    \
  IMP_IMPLEMENT(ParticlesTemp get_output_particles(Particle*) const);   \
  IMP_IMPLEMENT(ContainersTemp get_input_containers(Particle*) const);  \
  IMP_IMPLEMENT(ContainersTemp get_output_containers(Particle*) const); \
  IMP_OBJECT(Name)

//! Declare the functions needed for a QuadModifier
/** In addition to the methods done by IMP_OBJECT, it declares
    - IMP::QuadDerivativeModifier::apply()
    - IMP::QuadDerivativeModifier::get_input_particles()
    - IMP::QuadDerivativeModifier::get_output_particles()
*/
#define IMP_QUAD_DERIVATIVE_MODIFIER(Name)                        \
  IMP_IMPLEMENT(void apply(const ParticleQuad& a,\
                           DerivativeAccumulator&da) const);            \
  IMP_IMPLEMENT_INLINE(void apply_index(Model *m,                       \
                                        const ParticleIndexQuad& a,  \
                                        DerivativeAccumulator&da) const, { \
    return Name::apply(IMP::internal::get_particle(m,a), da);           \
                       });                                              \
  IMP_IMPLEMENT_INLINE(void apply_indexes(Model *m,\
                                          const ParticleIndexQuads &ps,    \
                                          DerivativeAccumulator&da) const, { \
    for (unsigned int i=0; i< ps.size(); ++i) {                         \
      Name::apply_index(m, ps[i], da);                                  \
    }                                                                   \
                       });                                              \
  IMP_IMPLEMENT(ParticlesTemp get_input_particles(Particle*) const);    \
  IMP_IMPLEMENT(ParticlesTemp get_output_particles(Particle*) const);   \
  IMP_IMPLEMENT(ContainersTemp get_input_containers(Particle*) const);  \
  IMP_IMPLEMENT(ContainersTemp get_output_containers(Particle*) const); \
  IMP_OBJECT(Name)



//! Declare the functions needed for a QuadModifier
/** In addition to the methods done by IMP_OBJECT, it declares
    - IMP::QuadModifier::apply(IMP::Particle*)
    - IMP::QuadModifier::get_input_particles()
    - IMP::QuadModifier::get_output_particles()
*/
#define IMP_INDEX_QUAD_MODIFIER(Name)                     \
  IMP_IMPLEMENT_INLINE(void apply(const ParticleQuad& a) const, {  \
    apply_index(IMP::internal::get_model(a),                            \
                IMP::internal::get_index(a));                           \
    });                                                                 \
  IMP_IMPLEMENT(void apply_index(Model *m,\
                                 const ParticleIndexQuad& a) const);   \
  IMP_IMPLEMENT(ParticlesTemp get_input_particles(Particle*) const);    \
  IMP_IMPLEMENT(ParticlesTemp get_output_particles(Particle*) const);   \
  IMP_IMPLEMENT(ContainersTemp get_input_containers(Particle*) const);  \
  IMP_IMPLEMENT(ContainersTemp get_output_containers(Particle*) const); \
  IMP_OBJECT(Name)

//! Declare the functions needed for a QuadModifier
/** In addition to the methods done by IMP_OBJECT, it declares
    - IMP::QuadDerivativeModifier::apply()
    - IMP::QuadDerivativeModifier::get_input_particles()
    - IMP::QuadDerivativeModifier::get_output_particles()
*/
#define IMP_INDEX_QUAD_DERIVATIVE_MODIFIER(Name)                        \
  IMP_IMPLEMENT_INLINE(void apply(const ParticleQuad& a,\
                                  DerivativeAccumulator&da) const, {    \
    apply_index(IMP::internal::get_model(a),                            \
                IMP::internal::get_index(a), da);                       \
                       });                                              \
  IMP_IMPLEMENT(void apply_index(Model *m, const ParticleIndexQuad& a,\
                                 DerivativeAccumulator&da) const);      \
  IMP_IMPLEMENT_INLINE(void apply_indexes(Model *m,\
                                          const ParticleIndexQuads &ps,    \
                                          DerivativeAccumulator&da) const, { \
    for (unsigned int i=0; i< ps.size(); ++i) {                         \
      Name::apply_index(m, ps[i], da);                                  \
    }                                                                   \
                       });                                              \
  IMP_IMPLEMENT(ParticlesTemp get_input_particles(Particle*) const);    \
  IMP_IMPLEMENT(ParticlesTemp get_output_particles(Particle*) const);   \
  IMP_IMPLEMENT(ContainersTemp get_input_containers(Particle*) const);  \
  IMP_IMPLEMENT(ContainersTemp get_output_containers(Particle*) const); \
  IMP_OBJECT(Name)




#ifndef IMP_DOXYGEN
#define IMP_IMPLEMENT_QUAD_CONTAINER(Name)                        \
  void apply(const QuadModifier *sm) const {                       \
    template_apply(sm);                                                 \
  }                                                                     \
  void apply(const QuadDerivativeModifier *sm,                     \
             DerivativeAccumulator &da) const {                         \
    template_apply(sm, da);                                             \
  }                                                                     \
  double evaluate(const QuadScore *s,                              \
                  DerivativeAccumulator *da) const {                    \
    return template_evaluate(s, da);                                    \
  }                                                                     \
  double evaluate_if_good(const QuadScore *s,                      \
                          DerivativeAccumulator *da, double max) const { \
    return template_evaluate_if_good(s, da, max);                       \
  }                                                                     \
  ParticlesTemp get_all_possible_particles() const;                     \
  IMP_OBJECT(Name)
#endif



/** Implement the needed template functions for a container.
    \param[in] Name the name
    \param[in] LOOP do the loop. There should be a line IMP_OPERATION and
    the current item should be in a variable named item at that time.
 */
#define IMP_IMPLEMENT_QUAD_CONTAINER_OPERATIONS(Name, LOOP)       \
  public:                                                               \
  IMP_IMPLEMENTATION_TEMPLATE_1(class SM,                               \
  void template_apply(const SM *sm,                                     \
                      DerivativeAccumulator &da) const, {               \
                       LOOP(IMP::internal::call_apply_index(get_model(),\
                                                        sm, item, da)); \
                                });                                     \
  IMP_IMPLEMENTATION_TEMPLATE_1(class SM,                               \
    void template_apply(const SM *sm) const, {                          \
                      LOOP(IMP::internal::call_apply_index(get_model(), \
                                                        sm, item));     \
                                });                                     \
  IMP_IMPLEMENTATION_TEMPLATE_1(class SS,                               \
  double template_evaluate(const SS *s,                                 \
                           DerivativeAccumulator *da) const, {          \
    double ret=0;                                                       \
    LOOP({                                                              \
        double cur=IMP::internal::call_evaluate_index(get_model(),      \
                                                      s, item, da);     \
      ret+=cur;                                                         \
      });                                                               \
    return ret;                                                         \
                                });                                     \
  IMP_IMPLEMENTATION_TEMPLATE_1(class SS,                               \
  double template_evaluate_if_good(const SS *s,                         \
                                   DerivativeAccumulator *da,           \
                                   double max) const, {                 \
    double ret=0;                                                       \
    LOOP({                                                              \
        double cur=IMP::internal::call_evaluate_if_good_index(get_model(), \
                                               s, item, da, max);       \
      ret+=cur;                                                         \
      max-=cur;                                                         \
      if (max < 0) return ret;                                          \
      });                                                               \
    return ret;                                                         \
                                });


//! Declare the needed functions for a QuadContainer
/** In addition to the methods of IMP_OBJECT, it declares
    - IMP::QuadContainer::get_contains_particle_particle_quad()
    - IMP::QuadContainer::get_number_of_particle_particle_quads()
    - IMP::QuadContainer::get_particle_particle_quad()
    - IMP::QuadContainer::apply()
    - IMP::QuadContainer::evaluate()
    - IMP::Interaction::get_input_objects()
*/
#define IMP_QUAD_CONTAINER(Name)                                  \
  IMP_IMPLEMENT(bool get_is_changed() const);                           \
  IMP_IMPLEMENT(bool get_contains_particle_quad(const ParticleQuad& p)   \
                const);                                                 \
  IMP_IMPLEMENT(ParticleIndexQuads get_indexes() const);                   \
  IMP_IMPLEMENT(ParticleIndexQuads get_all_possible_indexes() const);      \
  IMP_IMPLEMENT(void do_before_evaluate());                             \
  IMP_IMPLEMENT(ParticlesTemp get_input_particles() const);             \
  IMP_IMPLEMENT(ContainersTemp get_input_containers() const);           \
  IMP_IMPLEMENT_QUAD_CONTAINER(Name)


//! Declare the needed functions for an active QuadContainer
/** In addition to the methods of IMP_QUAD_CONTAINER(), it declares
    - IMP::ScoreState::get_input_particles()
    - IMP::ScoreState::get_input_containers()
    - IMP::ScoreState::do_before_evaluate()
*/
#define IMP_ACTIVE_QUAD_CONTAINER(Name)                           \
  IMP_QUAD_CONTAINER(name)

/** These macros avoid various inefficiencies.

    The macros take the name of the sequence and the operation to
    peform. The item in the sequence is called _1, it's index is _2.
    Use it like
    \code
    IMP_FOREACH_PARTICLE(sc, std::cout << "Item " << _2
                         << " is _1->get_name() << std::endl);
    \endcode
*/
#define IMP_FOREACH_QUAD(sequence, operation) do {                \
    IMP::ParticleQuadsTemp imp_all=sequence->get();   \
  for (unsigned int _2=0;                                               \
       _2 != imp_all.size();                                            \
       ++_2) {                                                          \
    IMP::ParticleQuad _1= imp_all[_2];               \
    bool imp_foreach_break=false;                                       \
    operation                                                           \
      if (imp_foreach_break) break;                                     \
  }                                                                     \
  } while (false)



/** These macros avoid various inefficiencies.

    The macros take the name of the sequence and the operation to
    peform. The item in the sequence is called _1, it's index is _2.
    Use it like
    \code
    IMP_FOREACH_PARTICLE(sc, std::cout << "Item " << _2
                         << " is _1->get_name() << std::endl);
    \endcode
*/
#define IMP_FOREACH_QUAD_INDEX(sequence, operation)               \
  do {                                                                  \
    if (sequence->get_provides_access()) {                              \
      const ParticleIndexQuads &imp_foreach_access                         \
        =sequence->get_access();                                        \
      for (unsigned int _2=0; _2< imp_foreach_access.size(); ++_2) {    \
        IMP::ParticleIndexQuad _1= imp_foreach_access[_2];          \
        bool imp_foreach_break=false;                                   \
        operation                                                       \
          if (imp_foreach_break) { break;}                              \
      }                                                                 \
    } else {                                                            \
      ParticleIndexQuads imp_foreach_indexes              \
        =sequence->get_indexes();                                       \
      for (unsigned int _2=0;                                           \
           _2 != imp_foreach_indexes.size();                            \
           ++_2) {                                                      \
        IMP::ParticleIndexQuad _1= imp_foreach_indexes[_2];            \
        bool imp_foreach_break=false;                                   \
        operation                                                       \
          if (imp_foreach_break) break;                                 \
      }                                                                 \
    }                                                                   \
  } while (false)

#endif  /* IMPKERNEL_QUAD_MACROS_H */
