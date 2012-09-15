#include <ros/ros.h>
#include <ros/console.h>
#include <ros/topic.h>
#include <sensor_msgs/PointCloud2.h>

#include <hbrs_srvs/MakeBoundingBox.h>
#include <bounding_box_visualizer.h>

#include "helpers.hpp"
#include "bounding_box.h"

using namespace hbrs::visualization;

class BoundingBoxMakerNode
{

public:

  BoundingBoxMakerNode()
  : bounding_box_visualizer_("bounding_box", Color::SEA_GREEN)
  {
    ros::NodeHandle nh;
    make_bounding_box_server_ = nh.advertiseService("make_bounding_box", &BoundingBoxMakerNode::makeBoundingBoxCallback, this);
    ROS_INFO("Started [make_bounding_box] service.");
  }

private:

  bool makeBoundingBoxCallback(hbrs_srvs::MakeBoundingBox::Request& request, hbrs_srvs::MakeBoundingBox::Response& response)
  {
    ROS_INFO("Received [make_bounding_box] request.");
    PointCloud::Ptr cloud(new PointCloud);
    pcl::fromROSMsg(request.cloud, *cloud);
    const Eigen::Vector3f normal(request.axis.x, request.axis.y, request.axis.z);
    BoundingBox box = BoundingBox::create<PointT>(cloud->points, normal);
    convertBoundingBox(box, response.bounding_box);
    bounding_box_visualizer_.publish(response.bounding_box);
    return true;
  }

  ros::ServiceServer make_bounding_box_server_;

  BoundingBoxVisualizer bounding_box_visualizer_;

};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "bounding_box_maker");
  BoundingBoxMakerNode bbmn;
  ros::spin();
  return 0;
}

