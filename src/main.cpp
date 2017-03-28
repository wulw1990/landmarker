#include <iostream>
#include <string>
#include <set>
#include "opencv2/opencv.hpp"
#include "python.hpp"
using namespace std;
using namespace cv;

void Help() {
  cout << "Two parameter needed:" << endl;
  cout << "  1) image folder" << endl;
  cout << "  2) label folder" << endl;
}

static const int kWindowWidth = 1700;
static const int kWindowHeight = 900;

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
void PrintList(const vector<string>& list, string name) {
  cout << name << ": " << list.size() << endl;
  for (size_t i = 0; i < list.size(); ++i) {
    cout << "  " << i << "/" << list.size() << ": " << list[i] << endl;
  }
}
void Label(string image_name, string label_name) {  //
  cout << "Label" << endl;
  Mat img = imread(image_name);
  if (image_name.empty()) {
    cerr << "image read failed: " << image_name << endl;
    exit(-1);
  }

  // resize img for window
  double scale = 1.0;
  if (img.cols > kWindowWidth) {
    double tmp = (double)kWindowWidth / img.cols;
    scale *= tmp;
    resize(img, img, Size(img.cols * tmp, img.rows * tmp));
  }
  if (img.rows > kWindowHeight) {
    double tmp = (double)kWindowHeight / img.rows;
    scale *= tmp;
    resize(img, img, Size(img.cols * tmp, img.rows * tmp));
  }

  imshow("Label", img);
  waitKey(0);
}
int main(int argc, char** argv) {
  if (argc != 3) {
    Help();
    return -1;
  }
  string path_image(argv[1]);
  string path_label(argv[2]);
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

  PrintList(list_image, "list_image");
  PrintList(list_label, "list_label");
  PrintList(list_unlabeled, "list_unlabeled");

  cout << "labeling" << endl;
  python::makedirs(path_label);
  for (size_t i = 0; i < list_unlabeled.size(); ++i) {
    cout << "  labeling " << i << "/" << list_unlabeled.size() << ": "
         << list_unlabeled[i] << endl;
    Label(path_image + "/" + list_unlabeled[i],
          path_label + "/" + list_unlabeled[i] + ".txt");
  }
}