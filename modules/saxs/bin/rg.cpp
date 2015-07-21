/**
   This is the program for computation of radius of gyration from SAXS profiles.
 */
#include <IMP/Model.h>
#include <IMP/Vector.h>
#include <IMP/atom/pdb.h>

#include <IMP/saxs/Profile.h>
#include <IMP/saxs/utility.h>

#include <fstream>
#include <vector>
#include <string>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace IMP::saxs;

int main(int argc, char **argv) {
  // output arguments
  for (int i = 0; i < argc; i++) std::cerr << argv[i] << " ";
  std::cerr << std::endl;

  double end_q_rg = 1.3;
  int multi_model_pdb = 1;
  po::options_description desc(
      "Usage: <pdb_file1> <pdb_file2> ... <profile_file1> <profile_file2> ...");
  desc.add_options()
    ("help", "Any number of input PDBs and profiles is supported")
    ("input-files", po::value<std::vector<std::string> >(),
     "input PDB and profile files")
    ("end_q_rg,q*rg", po::value<double>(&end_q_rg)->default_value(1.3),
     "end q*rg value for rg computation, q*rg<end_q_rg (default = 1.3), \
use 0.8 for elongated proteins")
    ("multi-model-pdb,m", po::value<int>(&multi_model_pdb)->default_value(1),
     "1 - read the first MODEL only (default), \
2 - read each MODEL into a separate structure, \
3 - read all models into a single structure");

  po::positional_options_description p;
  p.add("input-files", -1);
  po::variables_map vm;
  po::store(
      po::command_line_parser(argc, argv).options(desc).positional(p).run(),
      vm);
  po::notify(vm);

  std::vector<std::string> files, pdb_files, dat_files;
  if (vm.count("input-files")) {
    files = vm["input-files"].as<std::vector<std::string> >();
  }
  if (vm.count("help") || files.size() == 0) {
    std::cout << desc << "\n";
    return 0;
  }

  // 1. read pdbs and profiles, prepare particles
  std::vector<IMP::Particles> particles_vec;
  Profiles exp_profiles;

  read_files(files, pdb_files, dat_files, particles_vec, exp_profiles,
             false, true, multi_model_pdb, 0.5);

  // 2. compute rg for input profiles
  for (unsigned int i = 0; i < dat_files.size(); i++) {
    IMP::saxs::Profile *exp_saxs_profile = exp_profiles[i];
    double rg = exp_saxs_profile->radius_of_gyration(end_q_rg);
    std::cout << dat_files[i] << " Rg= " << rg << std::endl;
  }

  // 3. compute rg for input pdbs
  for (unsigned int i = 0; i < pdb_files.size(); i++) {
    double rg = IMP::saxs::radius_of_gyration(particles_vec[i]);
    std::cout << pdb_files[i] << " Rg= " << rg << std::endl;
  }
  return 0;
}
