#include "string-util.h"
#include "util/exceptions.h"
#include <fstream>
using namespace std;

namespace fract {

string ReadFile(const string &file) {
  const int bufsize = 4096;
  char buf[bufsize];
  ifstream in(file.c_str());
  if(in.fail())
    throw IOException("Can't open file " + file);
  int c;
  string res;
  do {
    in.read(buf, bufsize);
    c = (int) in.gcount();
    res.append(buf, c);
  } while (c == bufsize);
  return res;
}

vector<string> Tokenize(string &s, const string &delims) {
	vector<string> res;
	res.push_back("");
	for (int i = 0; i < (int)s.length(); ++i) {
		if (delims.find(s[i]) == string::npos) {
			res.back() += s[i];
		} else {
			res.push_back("");
		}
	}
	return res;
}

std::string RemoveFileNameFromPath(const string &s) {
	string::size_type p = s.find_last_of("/\\");
	if (p == string::npos)
		return "";
	return s.substr(0,p + 1);
}

std::string CombinePaths(string path1, string path2) {
	if (path1 == "")
		return path2;
	char l = path1[path1.length() - 1];
	if (l != '/' && l != '\\')
		path1 += "/";
	return path1 + path2;
}

}
