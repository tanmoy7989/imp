/**
 *  \file GroupnameContainer.cpp   \brief Container for classname.
 *
 *  This file is generated by a script (core/tools/make-container).
 *  Do not edit directly.
 *
 *  Copyright 2007-2010 IMP Inventors. All rights reserved.
 *
 */

#include "IMP/GroupnameContainer.h"
#include "IMP/internal/utility.h"
#include "IMP/GroupnameModifier.h"

IMP_BEGIN_NAMESPACE

namespace {
  unsigned int next_index=0;
}


GroupnameContainer::GroupnameContainer(std::string name):
  Container(internal::make_object_name(name, next_index++)){
}

// here for gcc
GroupnameContainer::~GroupnameContainer(){
}

IMP_END_NAMESPACE
