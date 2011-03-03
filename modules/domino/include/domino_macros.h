/**
 *  \file domino_macros.h    \brief Various important macros
 *                           for implementing decorators.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPDOMINO_MACROS_H
#define IMPDOMINO_MACROS_H

/** This macro declares
    - IMP::domino::ParticleStates::get_number_of_states()
    - IMP::domino::ParticleStates::load_state()
    and defines
    - IMP::domino::after_set_current_particle() to be empty
*/
#define IMP_PARTICLE_STATES(Name)                                       \
  public:                                                               \
 virtual unsigned int get_number_of_particle_states() const;            \
 virtual void load_particle_state(unsigned int, Particle*) const;       \
 IMP_OBJECT(Name)


/** This macro declares
    - IMP::domino::SubsetStateTable::get_subset_states()
*/
#define IMP_SUBSET_STATES_TABLE(Name)                                   \
  public:                                                               \
  virtual IMP::domino::SubsetStates                                     \
  get_subset_states(const IMP::domino::Subset&s) const;                 \
  IMP_OBJECT(Name)




/** This macro declares
    - IMP::domino::SubsetFilterTable::get_subset_filter()
*/
#define IMP_SUBSET_FILTER_TABLE(Name)                                   \
  public:                                                               \
  virtual IMP::domino::SubsetFilter*                                    \
  get_subset_filter(const IMP::domino::Subset&s,                        \
                    const IMP::domino::Subsets &excluded) const;        \
  virtual double get_strength(const IMP::domino::Subset&s,              \
                              const IMP::domino::Subsets &excluded) const; \
  IMP_OBJECT(Name)

/** This macro defines a class NameSubsetFilterTable from a method
    which is applied to disjoint sets. The code should assume there is
    a SubsetState state and an Ints members which ordered indices into
    the SubsetState for the current set.
*/
#define IMP_DISJOINT_SUBSET_FILTER_TABLE_DECL(Name)                     \
  class IMPDOMINOEXPORT Name##SubsetFilterTable:                        \
    public DisjointSetsSubsetFilterTable {                              \
    typedef DisjointSetsSubsetFilterTable P;                            \
  public:                                                               \
    Name##SubsetFilterTable(IMP::domino::ParticleStatesTable *pst):     \
      P(pst, std::string(#Name)+std::string(" %1%")){}                  \
    Name##SubsetFilterTable(): P(std::string(#Name)                     \
                               +std::string(" %1%")){}                  \
    IMP_SUBSET_FILTER_TABLE(Name##SubsetFilterTable);                   \
  };                                                                    \
  IMP_OBJECTS(Name##SubsetFilterTable, Name##SubsetFilterTables)


#define IMP_DISJOINT_SUBSET_FILTER_TABLE_DEF(Name, filter, strength)    \
  struct Name##Filter {                                                 \
    bool operator()(const SubsetState &state,                           \
                    const Ints &members) const {                        \
      filter;                                                           \
    }                                                                   \
  };                                                                    \
  struct Name##Strength {                                               \
    double operator()(const Subset &s,                                  \
                      const Subsets &excluded,                          \
                      const Ints &members) const {                      \
      strength;                                                         \
    }                                                                   \
  };                                                                    \
  void Name##SubsetFilterTable::do_show(std::ostream &) const {         \
  }                                                                     \
  IMP::domino::SubsetFilter* Name##SubsetFilterTable::                  \
  get_subset_filter(const IMP::domino::Subset &s,                       \
                    const IMP::domino::Subsets &excluded) const{        \
    set_was_used(true);                                                 \
    std::vector<Ints> all;                                              \
    Ints used;                                                          \
    get_indexes(s, excluded, all, used);                                \
    return get_disjoint_set_filter<Name##Filter>(#Name, all, used);     \
  }                                                                     \
  double                                                                \
  Name##SubsetFilterTable::get_strength(const IMP::domino::Subset &s,   \
                        const IMP::domino::Subsets &excluded) const{    \
    set_was_used(true);                                                 \
    std::vector<Ints> all;                                              \
    Ints used;                                                          \
    get_indexes(s, excluded, all, used);                                \
    return get_disjoint_set_strength<Name##Strength>(s, excluded, all,used); \
  }



/** This macro declares
    - IMP::domino::SubsetFilter::get_is_ok()
*/
#define IMP_SUBSET_FILTER(Name)                                         \
  public:                                                               \
  virtual bool get_is_ok(const IMP::domino::SubsetState& state) const;  \
 IMP_OBJECT(Name)


/** This macro declares
    - IMP::domino::DiscreteSampler::do_get_sample_states()
*/
#define IMP_DISCRETE_SAMPLER(Name)                                      \
  public:                                                               \
  SubsetStates do_get_sample_states(const IMP::domino::Subset &known) const; \
  IMP_OBJECT(Name)


/** This macro declares:
    - IMP::domino::SubsetGraphTable::get_subset_graph()
    - IMP::Object methods
 */
#define IMP_SUBSET_GRAPH_TABLE(Name)                                    \
  public:                                                               \
  SubsetGraph get_subset_graph(IMP::domino::ParticleStatesTable *pst) const; \
IMP_OBJECT(Name)

#endif  /* IMPDOMINO_MACROS_H */
