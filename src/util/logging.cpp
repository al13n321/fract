#include "logging.h"
#include "string-util.h"
#include "exceptions.h"

namespace fract {

std::string DumpWithLineNumbers(const std::string &content) {
  std::vector<std::string> lines = Tokenize(content, "\n\r");
  std::string path = "dumps/" + ToString(rand());
  std::string processed;
  for (size_t i = 0; i < lines.size(); ++i) {
    processed += ToString(i + 1);
    processed += ": ";
    processed += lines[i];
    processed += "\n";
  }
  WriteFile(path, processed);
  return path;
}

// Removes oldest files in directory with dumps.
void CleanupDumps() {
  // TODO: implement and call.
  throw NotImplementedException("CleanupDumps() is not implemented");
}

}
