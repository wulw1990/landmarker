#include "labeler.hpp"

#include <fstream>
#include <iostream>
#include <vector>
#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

static const int kWindowWidth = 1700;
static const int kWindowHeight = 900;
static const char* kWindowName = "Labeler";
static const int kLandmarkSize = 4;

struct RectData {
  Mat img;
  Rect rect;
  bool ok;
  RectData() {
    rect = Rect(0, 0, 0, 0);
    ok = false;
  }
};
void OnMouseRect(int event, int x, int y, int, void* data) {
  RectData* p = (RectData*)data;
  Mat mat;
  switch (event) {
    case EVENT_LBUTTONDOWN:
      if (p->rect.width != 0 && p->rect.height != 0) return;
      p->rect.x = x;
      p->rect.y = y;
      break;
    case EVENT_MOUSEMOVE:
      if (p->rect.x == 0 && p->rect.y == 0) return;
      if (x <= p->rect.x || y <= p->rect.y) return;
      p->rect.width = x - p->rect.x + 1;
      p->rect.height = y - p->rect.y + 1;
      mat = p->img.clone();
      rectangle(mat, p->rect, Scalar(255, 0, 0), 3);
      imshow(kWindowName, mat);
      break;
    case EVENT_LBUTTONUP:
      if (p->rect.x == 0 && p->rect.y == 0) return;
      if (x <= p->rect.x || y <= p->rect.y) return;
      p->rect.width = x - p->rect.x + 1;
      p->rect.height = y - p->rect.y + 1;
      p->ok = true;
  }
}
struct LandmarkData {
  Mat img;
  vector<Point2i> landmark;
};
void DrawLandmark(Mat img, vector<Point2i> landmark) {
  for (size_t i = 0; i < landmark.size(); ++i) {
    circle(img, landmark[i], 3, Scalar(255, 0, 0), -1);
  }
}
void OnMouseLandmark(int event, int x, int y, int, void* data) {
  LandmarkData* p = (LandmarkData*)data;
  Mat mat;
  switch (event) {
    case EVENT_LBUTTONDOWN:
      p->landmark.push_back(Point2i(x, y));
      cout << "    Add point: " << p->landmark[p->landmark.size() - 1]
           << ", size=" << p->landmark.size() << endl;
      mat = p->img.clone();
      DrawLandmark(mat, p->landmark);
      imshow(kWindowName, mat);
      break;
  }
}
Rect GetRect(Mat origin) {
  // resize img for window
  Mat img = origin.clone();
  double scale = 1.0;
  if (img.cols > kWindowWidth || img.rows > kWindowHeight) {
    scale =
        min((double)kWindowWidth / img.cols, (double)kWindowHeight / img.rows);
    resize(img, img, Size(img.cols * scale, img.rows * scale));
  }

  // get rect
  Rect rect;
  {
    RectData rect_data;
    imshow(kWindowName, img);
    rect_data.img = img.clone();
    while (1) {
      waitKey(33);
      cv::setMouseCallback(kWindowName, OnMouseRect, &rect_data);
      if (rect_data.ok) {
        break;
      }
    }
    rect = rect_data.rect;
    rect.x /= scale;
    rect.y /= scale;
    rect.width /= scale;
    rect.height /= scale;
  }
  return rect;
}
vector<Point2i> GetLandmark(Mat origin, Rect rect) {
  Mat roi = origin(rect).clone();

  double scale = 1.0;
  if (roi.cols < kWindowWidth || roi.rows < kWindowHeight) {
    scale =
        min((double)kWindowWidth / roi.cols, (double)kWindowHeight / roi.rows);
    resize(roi, roi, Size(roi.cols * scale, roi.rows * scale));
  }

  LandmarkData landmark_data;
  landmark_data.img = roi.clone();
  imshow(kWindowName, roi);
  while (1) {
    waitKey(33);
    cv::setMouseCallback(kWindowName, OnMouseLandmark, &landmark_data);
    if (landmark_data.landmark.size() >= kLandmarkSize) {
      landmark_data.landmark.resize(kLandmarkSize);
      break;
    }
  }
  for (size_t i = 0; i < landmark_data.landmark.size(); ++i) {
    landmark_data.landmark[i].x /= scale;
    landmark_data.landmark[i].y /= scale;
    landmark_data.landmark[i].x += rect.x;
    landmark_data.landmark[i].y += rect.y;
  }
  return landmark_data.landmark;
}
void ShowResult(Mat origin, vector<Point2i> landmark) {
  Mat img = origin.clone();
  double scale = 1.0;
  if (img.cols > kWindowWidth || img.rows > kWindowHeight) {
    scale =
        min((double)kWindowWidth / img.cols, (double)kWindowHeight / img.rows);
    resize(img, img, Size(img.cols * scale, img.rows * scale));
  }
  for (size_t i = 0; i < landmark.size(); ++i) {
    circle(img, Point2f(landmark[i].x * scale, landmark[i].y * scale), 3,
           Scalar(255, 0, 0), -1);
  }
  imshow(kWindowName, img);
  waitKey(0);
}
void Label(string image_name, string label_name) {  //
  Mat origin = imread(image_name);
  if (image_name.empty()) {
    cerr << "image read failed: " << image_name << endl;
    exit(-1);
  }

  Rect rect = GetRect(origin);
  vector<Point2i> landmark = GetLandmark(origin, rect);
  ShowResult(origin, landmark);

  ofstream fout(label_name.c_str());
  if (!fout.is_open()) {
    cerr << "can not write: " << label_name << endl;
    exit(-1);
  }
  fout << landmark.size() << endl;
  for (size_t i = 0; i < landmark.size(); ++i) {
    fout << landmark[i].x << " " << landmark[i].y << endl;
  }
}
void Check(std::string image_name, std::string label_name) {
  Mat origin = imread(image_name);
  if (image_name.empty()) {
    cerr << "image read failed: " << image_name << endl;
    exit(-1);
  }

  vector<Point2i> landmark;
  int num;
  ifstream fin(label_name.c_str());
  if (!fin.is_open()) {
    cerr << "can not read: " << label_name << endl;
    exit(-1);
  }
  fin >> num;
  landmark.resize(num);
  for (size_t i = 0; i < landmark.size(); ++i) {
    fin >> landmark[i].x >> landmark[i].y;
  }
  ShowResult(origin, landmark);
}
