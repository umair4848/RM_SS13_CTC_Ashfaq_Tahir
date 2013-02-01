#ifndef __TIEROSINTERFACE__
#define __TIEROSINTERFACE__

#include "ros/ros.h"
#include "hbrs_text_extraction/GetRawText.h"

class TIEROSInterface {
protected:
  ros::NodeHandle nh;
  ros::ServiceServer getTextServer;
  ros::Publisher OCRPublisher;
public:
  TIEROSInterface();
  virtual bool getTextCallback(hbrs_text_extraction::GetRawText::Request &request,
				hbrs_text_extraction::GetRawText::Response &response);
};

#endif
