#include <ros/ros.h>
#include <ros/console.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl/segmentation/extract_polygonal_prism_data.h>

#include <hbrs_srvs/AccumulateTabletopCloud.h>
#include "aliases.h"
#include "cloud_accumulation.h"
#include "helpers.hpp"

/** This node provides a service to accumulate parts of pointclouds that are
  * above some given planar polygon.
  *
  * In each pointcloud in the stream the algorithm extracts those points that
  * are above the given planar polygon and merges them into an occupancy octree.
  * The resulting cloud is then output to the user.
  *
  * Provides services:
  *   1) "accumulate_tabletop_cloud"
  *
  * Publishes:
  *   1) "accumulated_cloud"
  *      The cloud output as a response to the user of the service is also
  *      forwarded to this topic for the visualization purposes.
  *
  * Subscribes:
  *   1) "/camera/rgb/points"
  */
class TabletopCloudAccumulatorNode
{

public:

  TabletopCloudAccumulatorNode()
  {
    ros::NodeHandle nh;
    accumulate_service_ = nh.advertiseService("accumulate_tabletop_cloud", &TabletopCloudAccumulatorNode::accumulateCallback, this);
    accumulated_cloud_publisher_ = nh.advertise<sensor_msgs::PointCloud2>("accumulated_cloud", 1);
    ROS_INFO("Accumulate tabletop cloud service started.");
  }

private:

  bool accumulateCallback(hbrs_srvs::AccumulateTabletopCloud::Request& request, hbrs_srvs::AccumulateTabletopCloud::Response& response)
  {
    ROS_INFO("Accumulate tabletop cloud service requested.");
    updateConfiguration();
    PointCloud::Ptr polygon_cloud(new PointCloud);
    PlanarPolygon polygon;
    convertPlanarPolygon(request.polygon, polygon);
    polygon_cloud->points = polygon.getContour();
    eppd_.setInputPlanarHull(polygon_cloud);
    ca_.reset();
    ros::NodeHandle nh;
    ros::Subscriber subscriber = nh.subscribe("/camera/rgb/points", 1, &TabletopCloudAccumulatorNode::cloudCallback, this);

    // Wait some time while data is being accumulated.
    ros::Time timeout = ros::Time::now() + ros::Duration(accumulation_timeout_);
    while (ca_.getCloudCount() < accumulate_clouds_ && ros::Time::now() < timeout && ros::ok())
    {
      ros::spinOnce();
    }
    subscriber.shutdown();

    // Pack the response
    PointCloud cloud;
    cloud.header.frame_id = frame_id_;
    cloud.header.stamp = ros::Time::now();
    ca_.getAccumulatedCloud(cloud);
    pcl::toROSMsg(cloud, response.cloud);

    // Forward to the "accumulated_cloud" topic (if there are subscribers)
    if (accumulated_cloud_publisher_.getNumSubscribers())
      accumulated_cloud_publisher_.publish(response.cloud);

    return ca_.getCloudCount() != 0;
  }

  void cloudCallback(const sensor_msgs::PointCloud2::ConstPtr &ros_cloud)
  {
    PointCloud::Ptr cloud(new PointCloud);
    pcl::fromROSMsg(*ros_cloud, *cloud);
    frame_id_ = ros_cloud->header.frame_id;

    pcl::PointIndices::Ptr tabletop_indices(new pcl::PointIndices);
    eppd_.setInputCloud(cloud);
    eppd_.segment(*tabletop_indices);

    if (tabletop_indices->indices.size() == 0)
    {
      ROS_WARN("There are no points above the provided polygon.");
      return;
    }

    PointCloud::Ptr tabletop_cloud(new PointCloud);
    pcl::copyPointCloud(*cloud, *tabletop_indices, *tabletop_cloud);
    ca_.addCloud(tabletop_cloud);
  }

  void updateConfiguration()
  {
    ros::NodeHandle pn("~");

    // Extract polygonal prism settings
    double min_height, max_height;
    pn.param("min_height", min_height, 0.01);
    pn.param("max_height", max_height, 0.20);
    eppd_.setHeightLimits(min_height, max_height);

    // TODO: octree resolution parameter

    // Other settings
    pn.param("accumulation_timeout", accumulation_timeout_, 10);
    pn.param("accumulate_clouds", accumulate_clouds_, 1);
  }

  pcl::ExtractPolygonalPrismData<PointT> eppd_;
  CloudAccumulation ca_;
  std::string frame_id_;
  ros::ServiceServer accumulate_service_;
  ros::Publisher accumulated_cloud_publisher_;

  //PlanarPolygonPtr workspace_contour_;
  int accumulation_timeout_;
  int accumulate_clouds_;

};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "tabletop_cloud_accumulator_node");
  TabletopCloudAccumulatorNode tcan;
  ros::spin();
  return 0;
}

