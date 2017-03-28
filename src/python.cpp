#include "python.hpp"

#include <stdlib.h>
#include <sys/time.h>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

namespace python {
static vector<string> getLineListFromString(string str) {
  stringstream ss;
  ss << str;
  vector<string> list;
  while (getline(ss, str)) list.push_back(str);
  return list;
}
bool exec(std::string cmd, std::string* result) {
  // cmd += " 2>/dev/null";
  cmd += " 2>&1";
  FILE* pipe = popen(cmd.c_str(), "r");
  if (!pipe) return false;
  char buffer[128];
  *result = "";
  while (!feof(pipe)) {
    if (fgets(buffer, 128, pipe) != NULL) *result += buffer;
  }
  int status = pclose(pipe);
  // LOG(INFO) << "status=" << status;
  if (status != 0) {
    return false;
  }
  return true;
}
std::vector<std::string> listdir(std::string path) {
  string cmd;
  cmd = string("ls \"") + path + "\"";
  string result;
  vector<string> list;
  if (exec(cmd, &result)) {
    list = getLineListFromString(result);
  }
  return list;
}
void splitext(std::string path, std::string* root_, std::string* ext_) {
  size_t pos = path.rfind(".");
  string root, ext;
  if (pos == std::string::npos || pos == 0) {
    root = path;
    ext = "";
    return;
  }
  if (root_) *root_ = path.substr(0, pos);
  if (ext_) *ext_ = path.substr(pos, path.length() - pos);
}
void makedirs(string dir) {
  string cmd = "mkdir -p ";
  cmd += "\"" + dir + "\"";
  system(cmd.c_str());
}
}  // namespace python