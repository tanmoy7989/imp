/**
 *  \file KMLProxy.h   \brief proxy to k-means
 *
 *  Copyright 2007-8 Sali Lab. All rights reserved.
 *
 */

#ifndef IMPDOMINO_KML_PROXY_H
#define IMPDOMINO_KML_PROXY_H

#include "config.h"
#include <IMP/Particle.h>
#include <IMP/Model.h>
#include <vector>
#include <iostream>
#include <ctime>
#include <cmath>
#include <string>
#include "KMTerminationCondition.h"
#include "KMData.h"
#include "KMFilterCenters.h"
#include "KMLocalSearch.h"
#include "random_generator.h"
#include "KMLocalSearchLloyd.h"

IMPDOMINO_BEGIN_NAMESPACE

inline double elapsed_time(clock_t start) {
  return double(clock() - start)/double(CLOCKS_PER_SEC);
}

//! Proxy to apply k-means clustering on a set of Particles
class IMPDOMINOEXPORT KMLProxy
{
public:
  KMLProxy();
  //! Initialize the proxy
  /**
  /param[in] m IMP model
  /param[in] ps   The set of particles to apply the k-means on
  /param[in] atts The set of attributes of the particles
                  participating in the clustering
  /param[in] num_centers  The number of clusters
   */
  void initialize(Model *m,const Particles &ps,
       const std::vector<FloatKey> &atts,unsigned int num_centers);
  //! Run the k-means algorithm
  /**
  /param[in] ps a set of particles which are the initial k-means.
                If non are provided, the initial ones are set randomly.
   */
  void run(Particles *ps=NULL);
  std::string get_cmm_string() const;
  void log_header() const;
  void log_summary(KMFilterCenters *ctrs,Float run_time) const;
  //  Particles get_particles_for_center(unsigned int k) const;
  unsigned int get_particle_assignment(Particle *p) const;
  Particles get_centers() const {return centroids_;}
protected:
  void reset();
  bool is_init_;
  bool is_calculated;
  void set_default_values();

  //kmean data structure for handling the input data
  KMData *data_; // particles in KM data structure
  unsigned int dim_;// dimension
  unsigned int  data_size_;// data size
  unsigned int  kcenters_;// number of centers

  //kmean optimization parameters
  unsigned int  max_swaps_; // max number of swaps
  Float damp_factor_;// Lloyd's dampening factor
  unsigned int n_color_;
  bool new_clust_; // new clusters flag
  unsigned int max_dim_; // max flat dimension
  DistributionType distr_; // distribution //TODO - return when relevant
  Float std_dev_; // standard deviation
  Float corr_coef_; // correlation coef
  Float clus_sep_;//cluster seperation
  unsigned int max_visit_; // number of points visited
  unsigned int seed_; // seed for random numbers
  KMLocalSearchLloyd *lloyd_alg_; //Lloyd algorithm
  Float kc_build_time_; // execution times
  bool print_points_;// print points?
  bool show_assign_;// show point assignments?
  bool validate_;// validate point assignments?
  KMTerminationCondition term_;// termination parameters

  // our data
  Model *m_;
  Particles ps_; //input particles
  Particles centroids_; // the k means as particles
  std::vector<FloatKey> atts_; // the attributes k-means in minimized on
  std::map<Particle *,unsigned int> assignment_;
  // the assignment of input data to centers
};
IMPDOMINO_END_NAMESPACE
#endif /* IMPDOMINO_KML_PROXY_H */
