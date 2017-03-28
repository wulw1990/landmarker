#ifndef LANDMARKER_PYTHON_HPP_
#define LANDMARKER_PYTHON_HPP_

#include <vector>
#include <string>

namespace python {
std::vector<std::string> listdir(std::string path);
void splitext(std::string path, std::string* root, std::string* ext);
void makedirs(std::string dir);
}  // namespace python

#endif
