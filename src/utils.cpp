#include "utils.h"

#include <cctype>
#include <cmath>

using namespace std;

string_view Strip(string_view line) {
  while (!line.empty() && isspace(line.front())) {
    line.remove_prefix(1);
  }
  while (!line.empty() && isspace(line.back())) {
    line.remove_suffix(1);
  }
  return line;
}

bool IsZero(double x) {
  return abs(x) < 1e-6;
}

string ReadFileData(const string& file_name) {
  ifstream file(file_name, ios::binary | ios::ate);
  const ifstream::pos_type end_pos = file.tellg();
  file.seekg(0, ios::beg);

  string data(end_pos, '\0');
  file.read(&data[0], end_pos);
  return data;
}


std::string GuardedString(const std::string& str) {
  return "\""+str+"\"";
}


void PrintStringVectorGuarded(ostream& os, const vector<string>& vstr) {
  os << "[\n";
  bool first = true;
  for (const auto& s: vstr) {
    if (!first)
      os << ",\n";
    first = false;
    os << GuardedString(s);
  }
  os << "\n]";
}
