#include <ros/ros.h>
#include <ros/console.h>
#include <ros/topic.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl/filters/passthrough.h>

#include <hbrs_srvs/FindWorkspace.h>
#include "plane_extraction.h"

class WorkspaceFinderNode
{

public:

  WorkspaceFinderNode()
  {
    ros::NodeHandle nh;
    find_workspace_server_ = nh.advertiseService("find_workspace", &WorkspaceFinderNode::findWorkspaceCallback, this);
    ROS_INFO("Find workspace service started.");
  }

  virtual ~WorkspaceFinderNode() { };

private:

  bool findWorkspaceCallback(hbrs_srvs::FindWorkspace::Request& request, hbrs_srvs::FindWorkspace::Response& response)
  {
    ROS_INFO("Received find workspace request.");
    updateConfiguration();
    ROS_INFO("Waiting for a point cloud message...");
    auto ros_cloud = ros::topic::waitForMessage<sensor_msgs::PointCloud2>("/camera/rgb/points", ros::Duration(10));
    if (!ros_cloud)
    {
      ROS_ERROR("Timeout, no point cloud messages, aborting.");
      return false;
    }

    // Prepare point clouds.
    PointCloud::Ptr cloud(new PointCloud);
    PointCloud::Ptr cloud_filtered(new PointCloud);
    pcl::fromROSMsg(*ros_cloud, *cloud);

    // Run pass through filter.
    pass_through_->setInputCloud(cloud);
    pass_through_->filter(*cloud_filtered);

    PlanarPolygonVector planar_polygons;
    plane_extraction_.setInputCloud(cloud_filtered);
    plane_extraction_.extract(planar_polygons);
    ROS_INFO("Found %zu planes!", planar_polygons.size());

    if (!planar_polygons.size())
    {
      return false;
    }

    // TODO: find the one with the largest area
    response.stamp = ros_cloud->header.stamp;
    for (int i = 0; i < 4; ++i)
    {
      response.coefficients[i] = planar_polygons[0].getCoefficients()[i];
    }
    for (const auto& point : planar_polygons[0].getContour())
    {
      geometry_msgs::Point32 pt;
      pt.x = point.x;
      pt.y = point.y;
      pt.z = point.z;
      response.contour.push_back(pt);
    }
    return true;
  }

  void updateConfiguration()
  {
    ros::NodeHandle pn("~");
    double min_x, max_x;
    pass_through_.reset(new pcl::PassThrough<PointT>);
    pass_through_->setKeepOrganized(true);
    if (pn.getParam("min_x", min_x) && pn.getParam("max_x", max_x))
    {
      pass_through_->setFilterFieldName("x");
      pass_through_->setFilterLimits(min_x, max_x);
    }
    // TODO: other dimensions
    // TODO: plane constraints
  }

  ros::ServiceServer find_workspace_server_;
  std::unique_ptr<pcl::PassThrough<PointT>> pass_through_;
  PlaneExtraction plane_extraction_;

};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "workspace_finder_node");

  WorkspaceFinderNode wfn;

  ros::spin();

  return 0;
}

