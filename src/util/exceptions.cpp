#include "exceptions.h"
#include <iostream>

namespace fract {

void LogCurrentException() {
  try {
    throw;
  } catch (std::exception &e) {
    std::cerr << "exception: " << e.what() << std::endl;
  } catch (...) {
    std::cerr << "unknown exception" << std::endl;
  }
}

}
