#pragma once

#include <stdexcept>
#include <string>

namespace fract {

// Base class for exceptions. Can add stack trace here later.
class ExceptionBase {};

#define EXCEPTION_TYPE(name, base) \
  class name: public base, public ExceptionBase { \
   public: \
    name(const std::string &msg = #name): base(msg) {} \
  };

EXCEPTION_TYPE(NotImplementedException, std::logic_error);
EXCEPTION_TYPE(GLException, std::runtime_error);
EXCEPTION_TYPE(CLException, std::runtime_error);
EXCEPTION_TYPE(IOException, std::runtime_error);
EXCEPTION_TYPE(ShaderCompilationException, std::runtime_error);

#undef EXCEPTION_TYPE

}
