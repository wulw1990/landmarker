#include <iostream>
#include <set>
#include <string>
#include "labeler.hpp"
#include "python.hpp"
using namespace std;

void Help() {
  cout << "Three parameter needed:" << endl;
  cout << "  1) mode" << endl;
  cout << "  2) image folder" << endl;
  cout << "  3) label folder" << endl;
}
vector<string> FiltSuffix(const vector<string>& src,
                          const set<string>& suffix) {
  vector<string> dst;
  for (size_t i = 0; i < src.size(); ++i) {
    string ext;
    python::splitext(src[i], NULL, &ext);
    if (suffix.find(ext) != suffix.end()) {
      dst.push_back(src[i]);
    }
  }
  return dst;
}
vector<string> GetUnlabeledImageList(const vector<string>& list_image,
                                     const vector<string>& list_label) {
  set<string> set_label;
  for (size_t i = 0; i < list_label.size(); ++i) {
    set_label.insert(list_label[i]);
  }
  vector<string> list_unlabeled;
  for (size_t i = 0; i < list_image.size(); ++i) {
    string name = list_image[i] + ".txt";
    if (set_label.find(name) == set_label.end()) {
      list_unlabeled.push_back(list_image[i]);
    }
  }
  return list_unlabeled;
}
vector<string> GetLabeledImageList(const vector<string>& list_image,
                                   const vector<string>& list_label) {
  set<string> set_label;
  for (size_t i = 0; i < list_label.size(); ++i) {
    set_label.insert(list_label[i]);
  }
  vector<string> list_labeled;
  for (size_t i = 0; i < list_image.size(); ++i) {
    string name = list_image[i] + ".txt";
    if (set_label.find(name) != set_label.end()) {
      list_labeled.push_back(list_image[i]);
    }
  }
  return list_labeled;
}
void PrintList(const vector<string>& list, string name) {
  cout << name << ": " << list.size() << endl;
  for (size_t i = 0; i < list.size(); ++i) {
    cout << "  " << i << "/" << list.size() << ": " << list[i] << endl;
  }
}

int main(int argc, char** argv) {
  if (argc != 4) {
    Help();
    return -1;
  }
  string mode(argv[1]);
  string path_image(argv[2]);
  string path_label(argv[3]);
  cout << "path_image: " << path_image << endl;
  cout << "path_label: " << path_label << endl;

  vector<string> list_image = python::listdir(path_image);
  vector<string> list_label = python::listdir(path_label);

  set<string> suffix_image;
  suffix_image.insert(".jpg");
  suffix_image.insert(".JPG");
  suffix_image.insert(".png");
  suffix_image.insert(".PNG");
  suffix_image.insert(".jpeg");
  suffix_image.insert(".JPEG");

  set<string> suffix_label;
  suffix_label.insert(".txt");

  list_image = FiltSuffix(list_image, suffix_image);
  list_label = FiltSuffix(list_label, suffix_label);
  vector<string> list_unlabeled = GetUnlabeledImageList(list_image, list_label);
  vector<string> list_labeled = GetLabeledImageList(list_image, list_label);

  PrintList(list_image, "list_image");
  PrintList(list_label, "list_label");
  PrintList(list_unlabeled, "list_unlabeled");

  if (mode == "label") {
    if (list_unlabeled.empty()) return 0;
    cout << "labeling" << endl;
    python::makedirs(path_label);
    for (size_t i = 0; i < list_unlabeled.size(); ++i) {
      cout << "  labeling " << i << "/" << list_unlabeled.size() << ": "
           << list_unlabeled[i] << endl;
      Label(path_image + "/" + list_unlabeled[i],
            path_label + "/" + list_unlabeled[i] + ".txt");
    }
  } else if (mode == "check") {
    cout << "checking" << endl;
    for (size_t i = 0; i < list_labeled.size(); ++i) {
      cout << "  checking " << i << "/" << list_labeled.size() << ": "
           << list_labeled[i] << endl;
      Check(path_image + "/" + list_labeled[i],
            path_label + "/" + list_labeled[i] + ".txt");
    }
  } else {
    cerr << "error mode: " << mode << endl;
    return -1;
  }
}