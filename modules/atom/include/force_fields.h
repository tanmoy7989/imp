/**
 * \file force_fields \brief
 *
 * Copyright 2007-8 Sali Lab. All rights reserved.
 *
 */
#ifndef IMPATOM_FORCE_FIELDS_H
#define IMPATOM_FORCE_FIELDS_H

#include "config.h"
#include "Hierarchy.h"

#include <string>

IMPATOM_BEGIN_NAMESPACE

/** Add bonds using definitions from topology file
    if topology file is not given, default file is used
*/
IMPATOMEXPORT void add_bonds(Hierarchy d,
                             std::string topology_file_name = std::string());

/** Add vdW radius from Charmm parameter file.
    if files are not given, default files are used
*/
IMPATOMEXPORT void add_radius(Hierarchy d,
                              std::string par_file_name = std::string(),
                              std::string top_file_name = std::string());


IMPATOM_END_NAMESPACE

#endif /* IMPATOM_FORCE_FIELDS_H */
