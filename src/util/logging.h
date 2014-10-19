#pragma once

#include <string>

namespace fract {

// TODO: Maybe add some proper logging mechanism here
//       and use it instead of cerrs everywhere.

// Creates a file with unique name in a hard-coded directory
// and writes content to it, prepending line number to each line.
// Returns path to created file.
std::string DumpWithLineNumbers(const std::string &content);

// Removes oldest files in directory with dumps.
void CleanupDumps();

}
