#include "camera.hpp"

namespace io
{

Camera::Camera()
{
  int ret;
  MV_CC_DEVICE_INFO_LIST device_list;

  // 加了一个GigE掩码，用来玩一下虚拟相机
  ret = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &device_list);
  if (ret != MV_OK) {
    throw CameraError("Error when try to get device list");
  }

  if (device_list.nDeviceNum == 0) {
    std::cout << "MVS cannot find any device, please check the cameras" << std::endl;
    return;
  }

  // read camera info from MVS
  for (int i = 0; i < device_list.nDeviceNum; i++) {
    MV_CC_DEVICE_INFO * info = device_list.pDeviceInfo[i];
    MV_USB3_DEVICE_INFO & usb3_info = info->SpecialInfo.stUsb3VInfo;
    printf("camera %d: version %d\n", i, info->nMajorVer);

    printf("usb3 info:\n");
    printf("  DeviceNumber: %d\n", usb3_info.nDeviceNumber);
    printf("  VendorName: %s\n", usb3_info.chVendorName);
    printf("  ModelName: %s\n", usb3_info.chModelName);
    printf("  FamilyName: %s\n", usb3_info.chFamilyName);
    printf("  DeviceVersion: %s\n", usb3_info.chDeviceVersion);
    printf("  ManufacturerName: %s\n", usb3_info.chManufacturerName);
    printf("  SerialNumber: %s\n\n", usb3_info.chSerialNumber);
  }
  printf("Default select camera 0\n");

  ret = MV_CC_CreateHandle(&handle_, device_list.pDeviceInfo[0]);
  if (ret != MV_OK) {
    throw CameraError("Error when try to create handle");
  }

  ret = MV_CC_OpenDevice(handle_);
  if (ret != MV_OK) {
    throw CameraError("Error when try to open device");
  }

  // Change the virtual camera stream format to Bayer
  MV_CC_SetEnumValue(handle_, "PixelFormat", PixelType_Gvsp_BayerRG8);
  ret = MV_CC_StartGrabbing(handle_);
  if (ret != MV_OK) {
    throw CameraError("Error when try to start grabbing");
  }
}

cv::Mat Camera::read()
{
  // copy from example transfer ~ ~ ~
  MV_FRAME_OUT raw;
  unsigned int nMsec = 100;

  int ret;
  ret = MV_CC_GetImageBuffer(handle_, &raw, nMsec);
  if (ret != MV_OK) {
    throw CameraError("Error when try to get image buffer");
  }

  MV_CC_PIXEL_CONVERT_PARAM cvt_param;
  cv::Mat img(cv::Size(raw.stFrameInfo.nWidth, raw.stFrameInfo.nHeight), CV_8U, raw.pBufAddr);

  cvt_param.nWidth = raw.stFrameInfo.nWidth;
  cvt_param.nHeight = raw.stFrameInfo.nHeight;

  cvt_param.pSrcData = raw.pBufAddr;
  cvt_param.nSrcDataLen = raw.stFrameInfo.nFrameLen;
  cvt_param.enSrcPixelType = raw.stFrameInfo.enPixelType;

  cvt_param.pDstBuffer = img.data;
  cvt_param.nDstBufferSize = img.total() * img.elemSize();
  cvt_param.enDstPixelType = PixelType_Gvsp_BGR8_Packed;

  auto pixel_type = raw.stFrameInfo.enPixelType;
  const static std::unordered_map<MvGvspPixelType, cv::ColorConversionCodes> type_map = {
    {PixelType_Gvsp_BayerGR8, cv::COLOR_BayerGR2RGB},
    {PixelType_Gvsp_BayerRG8, cv::COLOR_BayerRG2RGB},
    {PixelType_Gvsp_BayerGB8, cv::COLOR_BayerGB2RGB},
    {PixelType_Gvsp_BayerBG8, cv::COLOR_BayerBG2RGB}};
  cv::cvtColor(img, img, type_map.at(pixel_type));

  MV_CC_FreeImageBuffer(handle_, &raw);
  return img;
}

Camera::~Camera()
{
  MV_CC_StopGrabbing(handle_);
  MV_CC_CloseDevice(handle_);
  MV_CC_DestroyHandle(handle_);
}

}  // namespace io