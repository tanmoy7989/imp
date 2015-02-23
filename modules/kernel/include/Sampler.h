/**
 *  \file IMP/kernel/Sampler.h     \brief Base class for all samplers.
 *
 *  Copyright 2007-2015 IMP Inventors. All rights reserved.
 *
 */

#ifndef IMPKERNEL_SAMPLER_H
#define IMPKERNEL_SAMPLER_H

#include <IMP/kernel/kernel_config.h>
#include "Model.h"
#include <IMP/base/Pointer.h>
#include <IMP/base/Object.h>
#include "ConfigurationSet.h"
#include <IMP/base/deprecation_macros.h>
#include <IMP/base/ref_counted_macros.h>

IMPKERNEL_BEGIN_NAMESPACE

//! Base class for all samplers.
/** A sampler takes a Model and searches for good configurations,
    given the optimizeable parameters and the scoring function in
    the Model and extra information that can be provided. Typically,
    the current configuration of the model is ignore.

    Typically a sampler works by using one or more Optimizer
    types to search for configurations which minimize the scoring
    function.
*/
class IMPKERNELEXPORT Sampler : public IMP::base::Object {
  base::PointerMember<Model> model_;
  base::PointerMember<ScoringFunction> sf_;

 public:
  Sampler(kernel::Model *m, std::string name = "Sampler %1%");
  ConfigurationSet *create_sample() const;

  ScoringFunction *get_scoring_function() const { return sf_; }
  void set_scoring_function(ScoringFunctionAdaptor sf);

  Model *get_model() const { return model_; }

 protected:
  virtual ConfigurationSet *do_sample() const = 0;

  // for the vtable
  IMP_REF_COUNTED_NONTRIVIAL_DESTRUCTOR(Sampler);
  //! Subclasses should override this method
};

IMP_OBJECTS(Sampler, Samplers);

IMPKERNEL_END_NAMESPACE

#endif /* IMPKERNEL_SAMPLER_H */
