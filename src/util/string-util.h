#pragma once
#include <string>
#include <vector>

namespace fract {

    std::string WstringToString(const std::wstring &wstr);
    std::wstring StringToWstring(const std::string &str);
 
	bool ReadFile(std::string file, std::string &res);

	// doesn't remove empty tokens
	std::vector<std::string> Tokenize(std::string &s, std::string delims);
    
	std::string RemoveFileNameFromPath(std::string path_with_file_name);
	std::string CombinePaths(std::string path1, std::string path2);

}
