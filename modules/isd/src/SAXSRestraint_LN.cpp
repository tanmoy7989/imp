/**
 *  \file SAXSRestraint_LN.h
 *  \brief Calculate score based on fit to SAXS profile.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 *
 */

#include <IMP/isd/SAXSRestraint_LN.h>
#include <IMP/isd/FNormal.h>

#include <IMP/log.h>

#include <IMP/atom/Hierarchy.h>
#include <IMP/core/LeavesRefiner.h>

IMPISD_BEGIN_NAMESPACE

SAXSRestraint_LN::SAXSRestraint_LN(const Particles& particles, const Scale& alpha,
        const Scale& gamma, const saxs::Profile& exp_profile, saxs::FormFactorType ff_type) :
  alpha_(alpha), gamma_(gamma),  exp_profile_(exp_profile), ff_type_(ff_type) {

  // for now just use a LeavesRefiner. It should, eventually, be a parameter
  // or a (not yet existing) AtomsRefiner.
  IMP::internal::OwnerPointer<Refiner> ref
    = new core::LeavesRefiner(atom::Hierarchy::get_traits());
  for(unsigned int i=0; i<particles.size(); i++) {
    if(core::RigidBody::particle_is_instance(particles[i])) {
      rigid_bodies_decorators_.push_back(
                              core::RigidBody::decorate_particle(particles[i]));
      rigid_bodies_.push_back(get_as<Particles>(ref->get_refined(rigid_bodies_decorators_.back())));
      // compute non-changing profile
     saxs::Profile rigid_part_profile;
      rigid_part_profile.calculate_profile(rigid_bodies_.back(), ff_type);
      rigid_bodies_profile_.add(rigid_part_profile);
    } else {
      if(atom::Atom::particle_is_instance(particles[i])) {
        particles_.push_back(particles[i]);
      }
    }
  }
  IMP_LOG(TERSE, "SAXS Restraint constructor: " << particles_.size()
          << " atom particles " << rigid_bodies_.size() << " rigid bodies\n");
}


ParticlesTemp SAXSRestraint_LN::get_input_particles() const
{
  ParticlesTemp pts(particles_.begin(), particles_.end());
  unsigned int sz=pts.size();
  for (unsigned int i=0; i< sz; ++i) {
    pts.push_back(atom::Hierarchy(pts[i]).get_parent());
  }
  for (unsigned int i=0; i< rigid_bodies_.size(); ++i) {
    pts.insert(pts.end(), rigid_bodies_[i].begin(), rigid_bodies_[i].end());
    for (unsigned int j=0; j< rigid_bodies_[i].size(); ++j) {
      // add the residue particle since that is needed too
      pts.push_back(atom::Hierarchy(rigid_bodies_[i][j]).get_parent());
    }
  }
  pts.push_back(alpha_);
  pts.push_back(gamma_);
  return pts;
}


ContainersTemp SAXSRestraint_LN::get_input_containers() const
{
  return ContainersTemp();
}



void SAXSRestraint_LN::compute_profile(saxs::Profile& model_profile) {
  // add non-changing profile
  model_profile.add(rigid_bodies_profile_);
  saxs::Profile profile(model_profile.get_min_q(),
                  model_profile.get_max_q(),
                  model_profile.get_delta_q());
  // compute inter-rigid bodies contribution
  for(unsigned int i=0; i<rigid_bodies_.size(); i++) {
    for(unsigned int j=i+1; j<rigid_bodies_.size(); j++) {
      profile.calculate_profile(rigid_bodies_[i], rigid_bodies_[j], ff_type_);
      model_profile.add(profile);
    }
  }
  // compute non rigid body particles contribution
  if(particles_.size() > 0) {
    profile.calculate_profile(particles_, ff_type_);
    model_profile.add(profile);
    // compute non rigid body particles - rigid bodies contribution
    for(unsigned int i=0; i<rigid_bodies_.size(); i++) {
      profile.calculate_profile(rigid_bodies_[i], particles_, ff_type_);
      model_profile.add(profile);
    }
  }
}

//! Calculate the score and the derivatives for particles of the restraint.
/** \param[in] acc If true (not NULL), partial first derivatives should be
                          calculated.
    \return score associated with this restraint for the given state of
            the model.
*/
double SAXSRestraint_LN::unprotected_evaluate(DerivativeAccumulator *acc) const
{
  IMP_LOG(TERSE, "SAXSRestraint_LN::unprotected_evaluate\n");

  /* compute Icalc */
  saxs::Profile model_profile(exp_profile_.get_min_q(),
                            exp_profile_.get_max_q(),
                            exp_profile_.get_delta_q());
  const_cast<SAXSRestraint_LN*>(this)->compute_profile(model_profile);

  //compute W
  std::vector<double> wx;
  double W=0;
  unsigned int profile_size = std::min(model_profile.size(), exp_profile_.size());
  for (unsigned int iq=0; iq<profile_size; iq++) {
      double err = exp_profile_.get_error(iq);
      double iexp = exp_profile_.get_intensity(iq);
      double val= square(iexp/err);
      wx.push_back(val);
      W += val;
  }
  //compute log(gammahat) and store logs
  double log_gammahat=0;
  std::vector<double> logs;
  for (unsigned int iq=0; iq<profile_size; iq++) {
      double Iexp = exp_profile_.get_intensity(iq);
      double Icalc = model_profile.get_intensity(iq);
      double val = log(Iexp/Icalc);
      logs.push_back(val);
      log_gammahat += wx[iq]*val;
  }
  log_gammahat = log_gammahat/W;

  //compute s^2
  double s2=0;
  for (unsigned int iq=0; iq<profile_size; iq++) {
      s2 += wx[iq] * square(logs[iq]-log_gammahat);
  }
  s2 = s2/W;

  // compute Scales
  double gamma_val=gamma_.get_scale();
  double alpha_val=alpha_.get_scale();

  //compute energy
  double score=0;
  score += profile_size*log(alpha_val);
  score += 0.5*profile_size*log(2*IMP::PI);
  for (unsigned int iq=0; iq<profile_size; iq++) {
      double err = exp_profile_.get_error(iq);
      score += log(err);
  }
  score += W/(2*square(alpha_val))*(square(log(gamma_val)-log_gammahat)+s2);

  if (!acc) return score;

  IMP_LOG(TERSE, "SAXS Restraint::compute derivatives\n");
  IMP_THROW("derivatives not implemented", ModelException);

  /*
  std::vector<IMP::algebra::VectorD<3> > derivatives;
  const FloatKeys keys = IMP::core::XYZ::get_xyz_keys();

  // 1. compute derivatives for each rigid body
  for(unsigned int i=0; i<rigid_bodies_.size(); i++) {
    if(!rigid_bodies_decorators_[i].get_coordinates_are_optimized()) continue;
    // contribution from other rigid bodies
    for(unsigned int j=0; j<rigid_bodies_.size(); j++) {
      if(i == j) continue;
      saxs_score_->compute_chi_derivative(model_profile, rigid_bodies_[i],
                                          rigid_bodies_[j], derivatives);
      for (unsigned int k = 0; k < rigid_bodies_[i].size(); k++) {
        rigid_bodies_[i][k]->add_to_derivative(keys[0],derivatives[k][0], *acc);
        rigid_bodies_[i][k]->add_to_derivative(keys[1],derivatives[k][1], *acc);
        rigid_bodies_[i][k]->add_to_derivative(keys[2],derivatives[k][2], *acc);
      }
    }
    if(particles_.size() > 0) {
      // contribution from other particles
      saxs_score_->compute_chi_derivative(model_profile, rigid_bodies_[i],
                                          particles_, derivatives);
      for (unsigned int k = 0; k < rigid_bodies_[i].size(); k++) {
        rigid_bodies_[i][k]->add_to_derivative(keys[0],derivatives[k][0], *acc);
        rigid_bodies_[i][k]->add_to_derivative(keys[1],derivatives[k][1], *acc);
        rigid_bodies_[i][k]->add_to_derivative(keys[2],derivatives[k][2], *acc);
      }
    }
  }
  // 2. compute derivatives for other particles
  if(particles_.size() > 0) {
    // particles own contribution
    compute_chi_derivative(model_profile, particles_, derivatives);
    for (unsigned int i = 0; i < particles_.size(); i++) {
      particles_[i]->add_to_derivative(keys[0], derivatives[i][0], *acc);
      particles_[i]->add_to_derivative(keys[1], derivatives[i][1], *acc);
      particles_[i]->add_to_derivative(keys[2], derivatives[i][2], *acc);
    }
    // rigid bodies contribution
    for(unsigned int i=0; i<rigid_bodies_.size(); i++) {
      saxs_score_->compute_chi_derivative(model_profile, particles_,
                                          rigid_bodies_[i], derivatives);
      for (unsigned int i = 0; i < particles_.size(); i++) {
        particles_[i]->add_to_derivative(keys[0], derivatives[i][0], *acc);
        particles_[i]->add_to_derivative(keys[1], derivatives[i][1], *acc);
        particles_[i]->add_to_derivative(keys[2], derivatives[i][2], *acc);
      }
    }
  }
  IMP_LOG(TERSE, "SAXS Restraint::done derivatives, score " << score << "\n");
  return score;
*/
}

void SAXSRestraint_LN::do_show(std::ostream&) const
{
//   out << "SAXSRestraint_LN: for " << particles_.size() << " particles "
//       << rigid_bodies_.size() << " rigid_bodies with sigma="  << sigma_ 
//       << " gamma=" << gamma_ << std::endl;
}

IMPISD_END_NAMESPACE
