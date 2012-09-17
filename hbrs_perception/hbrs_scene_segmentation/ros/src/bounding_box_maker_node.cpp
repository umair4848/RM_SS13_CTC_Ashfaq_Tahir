#include <ros/ros.h>
#include <ros/console.h>
#include <ros/topic.h>
#include <sensor_msgs/PointCloud2.h>

#include <hbrs_srvs/MakeBoundingBoxes.h>
#include <hbrs_msgs/BoundingBox.h>
#include <bounding_box_visualizer.h>

#include "helpers.hpp"
#include "bounding_box.h"

using namespace hbrs::visualization;

class BoundingBoxMakerNode
{

public:

  BoundingBoxMakerNode()
  : bounding_box_visualizer_("bounding_boxes", Color::SEA_GREEN)
  {
    ros::NodeHandle nh;
    make_bounding_box_server_ = nh.advertiseService("make_bounding_boxes", &BoundingBoxMakerNode::makeBoundingBoxesCallback, this);
    ROS_INFO("Started [make_bounding_boxes] service.");
  }

private:

  bool makeBoundingBoxesCallback(hbrs_srvs::MakeBoundingBoxes::Request& request, hbrs_srvs::MakeBoundingBoxes::Response& response)
  {
    ROS_INFO("Received [make_bounding_boxes] request.");
    const Eigen::Vector3f normal(request.axis.x, request.axis.y, request.axis.z);
    response.bounding_boxes.resize(request.clouds.size());
    for (size_t i = 0; i < request.clouds.size(); i++)
    {
      PointCloud::Ptr cloud(new PointCloud);
      pcl::fromROSMsg(request.clouds[i], *cloud);
      BoundingBox box = BoundingBox::create<PointT>(cloud->points, normal);
      convertBoundingBox(box, response.bounding_boxes[i]);
    }
    bounding_box_visualizer_.publish(response.bounding_boxes);
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

