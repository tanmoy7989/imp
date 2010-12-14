/**
 *  \file isd/NOERestraint.cpp
 *  \brief Restrain a list of particle pairs.
 *
 *  Copyright 2007-2010 IMP Inventors. All rights reserved.
 *
 */

#include <IMP/isd/NOERestraint.h>
#include <IMP/isd/FNormal.h>
#include <IMP/isd/Nuisance.h>
#include <IMP/core/XYZ.h>
#include <IMP/UnaryFunction.h>
#include <math.h>

IMPISD_BEGIN_NAMESPACE

NOERestraint::NOERestraint(Particle *p0, Particle *p1, 
			   Particle *sigma, Particle *gamma,
			   double Iexp) : p0_(p0), p1_(p1),
					  sigma_(sigma),
					  gamma_(gamma),
					  Iexp_(Iexp) {}
                                          
/* Apply the restraint to two atoms, two nuisances, one experimental value.
 */
double
NOERestraint::unprotected_evaluate(DerivativeAccumulator *accum) const
{
  core::XYZ d0(p0_);
  core::XYZ d1(p1_);
  double diff = (d0.get_coordinates()-d1.get_coordinates()).get_magnitude();
  double gamma_val=Nuisance(gamma_).get_nuisance();
  double sigma_val=Nuisance(sigma_).get_nuisance();
  double Icalc = gamma_val*pow(diff,-6);
  double Fx = log(Iexp_);
  double Fu = log(Icalc);
  double Jx = 1.0/Iexp_;
  IMP_NEW(FNormal, lognormal, (sigma_val,Fx,Fu,Jx)); 
  double score= lognormal->evaluate(0);

  if (accum)
  return score;
}

/* Return all particles whose attributes are read by the restraints. To
   do this, ask the pair score what particles it uses.*/
ParticlesTemp NOERestraint::get_input_particles() const
{
  ParticlesTemp ret;
  ret.push_back(p0_);
  ret.push_back(p1_);
  return ret;
}

/* The only container used is pc_. */
ContainersTemp NOERestraint::get_input_containers() const
{
  return ContainersTemp();
}

void NOERestraint::do_show(std::ostream& out) const
{
  out << "particle0= " << p0_->get_name() << std::endl;
  out << "particle1= " << p1_->get_name() << std::endl;
  out << "Iexp= " << Iexp_ << std::endl;
}

IMPISD_END_NAMESPACE
