#ifndef IO__CAMERA
#define IO__CAMERA
#include <opencv2/opencv.hpp>

#include "hikrobot/include/MvCameraControl.h"

namespace io 
{

class CameraBase
{
public:
  virtual ~CameraBase() = default;
  virtual cv::Mat read() = 0;
};

class CameraError : public std::runtime_error
{
public:
  CameraError(char * msg) : std::runtime_error(std::string(msg)) {}
  CameraError(std::string& msg) : std::runtime_error(msg) {}
};

class Camera : public CameraBase
{
public:
  Camera();
  cv::Mat read() override;
  ~Camera() override;
private:
  void * handle_;
};

} // namespace io
#endif
