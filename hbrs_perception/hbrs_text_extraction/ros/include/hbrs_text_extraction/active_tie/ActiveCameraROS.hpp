#ifndef __ACTIVE_CAMERA_ROS__
#define __ACTIVE_CAMERA_ROS__

#include <active-tie/ActiveCamera.hpp>
#include <active-tie/ActiveCameraValues.hpp>

#include "ros/ros.h"
#include "image_transport/image_transport.h"
#include "cv_bridge/CvBridge.h"
#include <active-tie/OpenCVIncludes.hpp>

/* Includes for dynamic reconfigure */
#include "dynamic_reconfigure/ConfigDescription.h"
#include "dynamic_reconfigure/Reconfigure.h"
#include "dynamic_reconfigure/Config.h"




namespace atie {
  
  class ActiveCameraROS : public ActiveCamera {
  private:
    ros::NodeHandle nh;
    ros::ServiceClient dynamicReconfigureClient;
    string confPath;
    bool transcientStateP;
    cv::Mat lastFrame;
    string reconfigureBase;
    image_transport::ImageTransport* iTransport;
    image_transport::Subscriber iSubscriber;
    sensor_msgs::CvBridge bridge;
    /* This allows us to wake until the setups have been acknowledged
       by the hardware */
    boost::posix_time::ptime parameterSetSyncTime;
    int transcientC;   

  public:
    ActiveCameraROS(char* imageTopic_, char* confTopic_, ActiveCameraValues* cv_= NULL);
    void imageCallback(const sensor_msgs::ImageConstPtr& msg_ptr);
    int setParameters(int n_, char** names_, int* values_);
    cv::Mat& getFrame();
    int getParameter(char* name_);
  };
};

#endif
