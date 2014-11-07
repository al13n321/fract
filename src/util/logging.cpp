#include "logging.h"
#include "string-util.h"
#include "exceptions.h"

#ifdef WIN32

#include <Windows.h>

#else

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#endif


namespace fract {

std::string DumpWithLineNumbers(const std::string &content) {
#ifdef WIN32
  if (!CreateDirectory("temp", nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
    throw IOException("can't create directory temp/");
#else
  struct stat st {};
  if (stat("temp", &st) == -1)
    mkdir("temp", 0777);
#endif

  std::vector<std::string> lines = Tokenize(content, "\n\r");
  std::string path = "temp/" + ToString(rand());
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
