/**
 *  \file random.cpp  \brief Random number generators used by IMP.
 *
 *  Copyright 2007-2015 IMP Inventors. All rights reserved.
 *
 */

#include "IMP/random.h"
#include "internal/base_static.h"

IMPKERNEL_BEGIN_NAMESPACE
boost::uint64_t get_random_seed() {
  return static_cast<boost::uint64_t>(internal::random_seed);
}

IMPKERNEL_END_NAMESPACE