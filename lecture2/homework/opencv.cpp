#include "opencv2/opencv.hpp"

#include "io/camera.hpp"
#include "tasks/apriltag_detector.hpp"
#include "tools/img_tools.hpp"

int main()
{
  // 初始化相机、yolo类
  const std::string yolo_config_path = "./configs/yolo.yaml";

  io::Camera camera;
  auto_charge::AprilTagDetector ad(yolo_config_path);

  while (1) {
    cv::Mat img = camera.read();

    auto ap_tags = ad.detect(img);
    for (auto & tag : ap_tags) {
      tools::draw_points(img, tag.corners, {0, 255, 0});
      tools::draw_text(img, std::to_string(tag.id), tag.center, {0, 255, 0});
    }

    cv::resize(img, img, cv::Size(640, 480));
    cv::imshow("img", img);
    if (cv::waitKey(1) == 'q') {
      break;
    }
  }

  return 0;
}