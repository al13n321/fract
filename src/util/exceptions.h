#pragma once

#include <stdexcept>

namespace fract {

// Base class for exceptions. Can add stack trace here later.
class ExceptionBase {};

#define EXCEPTION_TYPE(name, base) \
  class name: base, ExceptionBase { \
   public: \
    name(const std::string &msg = #name): base(msg) {} \
  };

EXCEPTION_TYPE(NotImplementedException, std::logic_error);
EXCEPTION_TYPE(VideocardAPICapabilityException, std::logic_error);

#undef EXCEPTION_TYPE

}
