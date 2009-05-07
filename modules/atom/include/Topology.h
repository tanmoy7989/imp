/**
 * \file Topology \brief topology definitions
 *
 * Copyright 2007-9 Sali Lab. All rights reserved.
 *
 */
#ifndef IMPATOM_TOPOLOGY_H
#define IMPATOM_TOPOLOGY_H

#include <IMP/base_types.h>
#include "ResidueDecorator.h"
#include "AtomDecorator.h"
#include "MolecularHierarchyDecorator.h"
#include "bond_decorators.h"

#include <fstream>
#include <map>
#include <vector>

IMPATOM_BEGIN_NAMESPACE

class IMPATOMEXPORT Topology {
public:
  //! Constructor with topology file
  Topology(const String& top_file_name);

  //! add bonds to the structure defined in the hierarchy
  void add_bonds(MolecularHierarchyDecorator mhd);

 private:
  class Bond {
  public:
    Bond(AtomType type1, AtomType type2,
         BondDecorator::Type bond_type = BondDecorator::COVALENT) :
      type1_(type1), type2_(type2), bond_type_(bond_type) {}
    AtomType type1_,type2_;
    BondDecorator::Type bond_type_;
  };

  void read_topology_file(std::ifstream& input_file);

  ResidueType parse_residue_line(const String& line);
  void parse_atom_line(const String& line, const ResidueType& curr_res_type);
  void parse_bond_line(const String& line, const ResidueType& curr_res_type);

  void add_bonds(ResidueDecorator rd);
  void add_bonds(MolecularHierarchyDecorator mhd,
                 MolecularHierarchyDecorator::Type type);
  void add_bonds(ResidueDecorator rd1, ResidueDecorator rd2);

 private:
  // map between imp_atom_type and charmm parameters (atom_type, charge)
  typedef std::map<AtomType, std::pair<std::string, float> > AtomTypeMap;

  // hash that maps between residue and atom name to charmm atom type
  // key1=imp_residue_type, key2= imp_atom_type, value=pair(atom_type, charge)
  std::map<ResidueType, AtomTypeMap> atom_res_type_2_charmm_atom_type_;

  // a list of residue bonds
  std::map<ResidueType, std::vector<Bond> > residue_bonds_;
};

IMPATOM_END_NAMESPACE

#endif /* IMPATOM_TOPOLOGY_H */
