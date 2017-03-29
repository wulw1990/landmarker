#ifndef LANDMARKER_LABELER_HPP_
#define LANDMARKER_LABELER_HPP_

#include <string>
#include <vector>
#include "opencv2/opencv.hpp"

class Labeler {
 public:
  static void Label(std::string image_name, std::string label_name);
  static void Check(std::string image_name, std::string label_name);

 private:
  // rect
  struct RectData {
    cv::Mat img;
    cv::Rect rect;
    bool ok;
    RectData() {
      rect = cv::Rect(0, 0, 0, 0);
      ok = false;
    }
  };
  static void OnMouseRect(int event, int x, int y, int, void* data);
  static cv::Rect GetRectFromUI(const cv::Mat& origin);

  // Landmark
  struct LandmarkData {
    cv::Mat img;
    std::vector<cv::Point2i> landmark;
  };
  static void DrawLandmarkWithIndex(cv::Mat img,
                                    std::vector<cv::Point2i> landmark);
  static void OnMouseLandmark(int event, int x, int y, int, void* data);
  static std::vector<cv::Point2i> GetLandmarkFromUI(const cv::Mat& origin,
                                                    cv::Rect rect);
  static void ShowResultLandmark(const cv::Mat& origin,
                                 std::vector<cv::Point2i> landmark, cv::Rect rect);

  static void SaveLandmark(std::string name,
                           const std::vector<cv::Point2i>& landmark,
                           const cv::Rect& rect);
  static void LoadLandmark(std::string name, std::vector<cv::Point2i>* landmark,
                           cv::Rect* rect);
};
#endif
