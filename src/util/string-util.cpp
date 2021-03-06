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

void WriteFile(const std::string &file, const std::string &contents) {
  ofstream out(file.c_str());
  if(out.fail())
    throw IOException("Can't open file " + file + " for writing");
  out << contents;
}

vector<string> Tokenize(const string &s, const string &delims) {
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

std::string PathConcat(const string &path1, const string &path2) {
	return PathConcat({path1, path2});
}
std::string PathConcat(std::initializer_list<std::string> paths) {
  std::string res;
  for (const std::string &path : paths) {
    if (res.empty())
      res = path;
    else {
      char l = res[res.length() - 1];
      if (l != '/' && l != '\\')
        res += "/";
      res += path;
    }
  }
  return res;
}

bool StartsWith(const std::string &string, const std::string &prefix) {
  return
    string.size() >= prefix.size() &&
    string.substr(0, prefix.size()) == prefix;
}

bool IsWhitespace(char c) {
  return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

std::string Strip(std::string s) {
  for (int i = 0; i < 2; ++i) {
    while (!s.empty() && IsWhitespace(s[s.size() - 1]))
      s.erase(s.end() - 1);
    std::reverse(s.begin(), s.end());
  }
  return s;
}

}
