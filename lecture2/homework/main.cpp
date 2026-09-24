#include "io/camera.hpp"
#include "opencv2/opencv.hpp"
#include "tasks/yolo.hpp"
#include "tools/img_tools.hpp"

int main()
{
  // 初始化相机、yolo类
  const std::string yolo_config_path = "./configs/yolo.yaml";

  io::Camera camera;
  auto_aim::YOLO yolo(yolo_config_path);

  while (1) {
    cv::Mat img = camera.read();

    // 调用yolo识别装甲板
    auto armors = yolo.detect(img);
    for(auto& armor : armors)
    {
      tools::draw_points(img, armor.points, {0, 255, 0});
    }

    cv::resize(img, img, cv::Size(640, 480));
    cv::imshow("img", img);
    if (cv::waitKey(0) == 'q') {
      break;
    }
  }

  return 0;
}