#ifndef __OLD_TIE_ROS_INTERFACE__
#define __OLD_TIE_ROS_INTERFACE__

#include <ros/ros.h>
#include "image_transport/image_transport.h"
#include <std_srvs/Empty.h>
#include <std_msgs/String.h>
#include <active_tie/TIEROSInterface.hpp>
#include <active-tie/OVActiveCamera.hpp>
#include <active-tie/OCRInterface.hpp>
#include <active-tie/OpenCVIncludes.hpp>
#include "cv_bridge/CvBridge.h"

class OldTIEROSInterface: public TIEROSInterface {
protected:
  atie::OVActiveCamera* camera;
  atie::AbbyOCR ocr;
  sensor_msgs::CvBridge bridge;
  image_transport::ImageTransport processImageTransport;
  image_transport::Publisher processImagePublisher;
  IplImage* processImageSmall;
public:
  OldTIEROSInterface(atie::OVActiveCamera* camera_);
  bool getTextCallback(hbrs_text_extraction::GetRawText::Request& request,
			hbrs_text_extraction::GetRawText::Response& response);  
};

#endif
