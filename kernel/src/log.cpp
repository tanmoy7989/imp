/**
 *  \file Log.cpp   \brief Logging and error reporting support.
 *
 *  Copyright 2007-2011 IMP Inventors. All rights reserved.
 *
 */

#include "IMP/log.h"
#include "IMP/exception.h"
#include "IMP/file.h"
#include "IMP/internal/static.h"

IMP_BEGIN_NAMESPACE

namespace {
  std::vector<std::string> contexts;
  std::vector<bool> context_initializeds;
  int context_last_initialized=-1;
}

void set_log_level(LogLevel l) {
  IMP_USAGE_CHECK(l >= SILENT && l < ALL_LOG,
            "Setting log to invalid level: " << l);
  internal::log_level=l;
}

void set_log_target(TextOutput l)
{
  internal::stream.set_stream(l);
}

TextOutput get_log_target()
{
  return internal::stream.get_stream();
}

IMPEXPORT void push_log_context(std::string name) {
  contexts.push_back(name);
  context_initializeds.push_back(false);
}

IMPEXPORT void pop_log_context() {
  if (context_initializeds.back()) {
    internal::log_indent-=2;
    std::string message= std::string("end ")
      +contexts.back()+"\n";
    internal::stream.write(message.c_str(), message.size());
    internal::stream.strict_sync();
  }
  contexts.pop_back();
  context_initializeds.pop_back();
  context_last_initialized= std::min<int>(context_last_initialized,
                                          contexts.size());
}


void add_to_log(std::string str) {
  IMP_INTERNAL_CHECK(static_cast<int>(internal::initialized)==11111111,
                     "You connot use the log before main is called.");
  while (static_cast<unsigned int>(context_last_initialized+1)
         < contexts.size()) {
    std::string message= std::string("begin ")
      +contexts[context_last_initialized+1]+":\n";
    internal::stream.write(message.c_str(), message.size());
    internal::log_indent+=2;
    context_initializeds[context_last_initialized+1]=true;
    ++context_last_initialized;
  }
  internal::stream.write(str.c_str(), str.size());
  internal::stream.strict_sync();
}

IMP_END_NAMESPACE
