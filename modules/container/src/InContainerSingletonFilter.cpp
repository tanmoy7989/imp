/**
 *  \file SingletonFilter.cpp   \brief Filter for singleton.
 *
 *  This file is generated by a script (core/tools/make-filter).
 *  Do not edit directly.
 *
 *  Copyright 2007-2012 Sali Lab. All rights reserved.
 *
 */

#include "IMP/container/InContainerSingletonFilter.h"

IMPCONTAINER_BEGIN_NAMESPACE


InContainerSingletonFilter
::InContainerSingletonFilter(SingletonContainer *c,
                             std::string name): SingletonPredicate(name),
                                                c_(c){}


IMPCONTAINER_END_NAMESPACE
