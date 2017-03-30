#include "labeler.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;

static const int kWindowWidth = 1700;
static const int kWindowHeight = 900;
static const char* kWindowName = "Labeler";
static const int kLandmarkSize = 8;

// Label -----------------------------------------------------------------------
void Labeler::Label(string image_name, string label_name) {  //
  Mat origin = imread(image_name);
  if (image_name.empty()) {
    cerr << "image read failed: " << image_name << endl;
    exit(-1);
  }

  Rect rect = GetRectFromUI(origin);
  cout << "    GetRectFromUI OK" << endl;

  vector<Point2i> landmark = GetLandmarkFromUI(origin, rect);
  cout << "    GetLandmarkFromUI OK" << endl;

  SaveLandmark(label_name, landmark, rect);
  cout << "    Save Label File OK" << endl;

  destroyAllWindows();
  ShowResultLandmark(origin, landmark, rect);
}

// Check -----------------------------------------------------------------------
void Labeler::Check(std::string image_name, std::string label_name) {
  Mat origin = imread(image_name);
  if (image_name.empty()) {
    cerr << "image read failed: " << image_name << endl;
    exit(-1);
  }

  vector<Point2i> landmark;
  Rect rect;
  LoadLandmark(label_name, &landmark, &rect);

  ShowResultLandmark(origin, landmark, rect);
}

// UI: get rect ----------------------------------------------------------------
void Labeler::OnMouseRect(int event, int x, int y, int, void* data) {
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
      break;
  }
}
Rect Labeler::GetRectFromUI(const Mat& origin) {
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
// UI: get landmark ------------------------------------------------------------
void Labeler::DrawLandmarkWithIndex(Mat img, vector<Point2i> landmark) {
  for (size_t i = 0; i < landmark.size(); ++i) {
    circle(img, landmark[i], 3, Scalar(0, 0, 255), -1);
    stringstream ss;
    ss << i % 4;
    putText(img, ss.str(), landmark[i], CV_FONT_HERSHEY_COMPLEX, 1,
            Scalar(255, 0, 0));
  }
  for (size_t i = 0; i < landmark.size() / 4; ++i) {
    int x = 0;
    int y = 0;
    for (int j = 0; j < 4; ++j) {
      x += landmark[i * 4 + j].x;
      y += landmark[i * 4 + j].y;
    }
    x /= 4;
    y /= 4;
    stringstream ss;
    ss << i;
    putText(img, ss.str(), Point2i(x, y), CV_FONT_HERSHEY_COMPLEX, 1,
            Scalar(0, 0, 244));
  }
}
void Labeler::OnMouseLandmark(int event, int x, int y, int, void* data) {
  if (event != EVENT_LBUTTONDOWN && event != EVENT_MOUSEMOVE) return;
  LandmarkData* p = (LandmarkData*)data;
  switch (event) {
    case EVENT_LBUTTONDOWN:
      p->landmark.push_back(Point2i(x, y));
      cout << "    Add point: " << p->landmark[p->landmark.size() - 1]
           << ", size=" << p->landmark.size() << endl;
      break;
  }
  Mat mat = p->img.clone();
  line(mat, Point2f(0, y), Point2f(mat.cols - 1, y), Scalar(127, 127, 127), 1);
  line(mat, Point2f(x, 0), Point2f(x, mat.rows - 1), Scalar(127, 127, 127), 1);
  DrawLandmarkWithIndex(mat, p->landmark);
  imshow(kWindowName, mat);
}
vector<Point2i> Labeler::GetLandmarkFromUI(const Mat& origin, Rect rect) {
  Mat roi = origin(rect).clone();

  double scale = 1.0;
  scale =
      min((double)kWindowWidth / roi.cols, (double)kWindowHeight / roi.rows);
  resize(roi, roi, Size(roi.cols * scale, roi.rows * scale));

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
void Labeler::ShowResultLandmark(const Mat& origin_, vector<Point2i> landmark_,
                                 Rect rect) {
  {
    Mat img = origin_.clone();
    double scale =
        min((double)kWindowWidth / img.cols, (double)kWindowHeight / img.rows);
    resize(img, img, Size(img.cols * scale, img.rows * scale));
    vector<Point2i> landmark = landmark_;
    for (size_t i = 0; i < landmark.size(); ++i) {
      landmark[i].x = scale * landmark[i].x;
      landmark[i].y = scale * landmark[i].y;
    }
    for (size_t i = 0; i < landmark.size(); ++i) {
      circle(img, landmark[i], 3, Scalar(0, 0, 255), -1);
    }
    imshow("Global", img);
    waitKey(33);
  }
  {
    Mat crop = origin_(rect).clone();
    vector<Point2i> landmark = landmark_;
    for (size_t i = 0; i < landmark.size(); ++i) {
      landmark[i].x -= rect.x;
      landmark[i].y -= rect.y;
    }
    double scale = min((double)kWindowWidth / crop.cols,
                       (double)kWindowHeight / crop.rows);
    resize(crop, crop, Size(crop.cols * scale, crop.rows * scale));
    for (size_t i = 0; i < landmark.size(); ++i) {
      landmark[i].x = scale * landmark[i].x;
      landmark[i].y = scale * landmark[i].y;
    }
    DrawLandmarkWithIndex(crop, landmark);
    imshow("Local", crop);
    waitKey(0);
  }
}

void Labeler::SaveLandmark(std::string name,
                           const std::vector<cv::Point2i>& landmark,
                           const cv::Rect& rect) {
  ofstream fout(name.c_str());
  if (!fout.is_open()) {
    cerr << "can not write: " << name << endl;
    exit(-1);
  }
  fout << rect.x << " " << rect.y << " " << rect.width << " " << rect.height
       << endl;
  fout << landmark.size() << endl;
  for (size_t i = 0; i < landmark.size(); ++i) {
    fout << landmark[i].x << " " << landmark[i].y << endl;
  }
  fout << "OK" << endl;
}
void Labeler::LoadLandmark(std::string name,
                           std::vector<cv::Point2i>* landmark_,
                           cv::Rect* rect) {
  std::vector<cv::Point2i>& landmark = *landmark_;
  int num;
  ifstream fin(name.c_str());
  if (!fin.is_open()) {
    cerr << "can not read: " << name << endl;
    exit(-1);
  }
  fin >> rect->x >> rect->y >> rect->width >> rect->height;
  fin >> num;
  landmark.resize(num);
  for (size_t i = 0; i < landmark.size(); ++i) {
    fin >> landmark[i].x >> landmark[i].y;
  }
  string ok;
  fin >> ok;
  if (ok != "OK") {
    cerr << "label file error: " << name << endl;
    exit(-1);
  }
}
