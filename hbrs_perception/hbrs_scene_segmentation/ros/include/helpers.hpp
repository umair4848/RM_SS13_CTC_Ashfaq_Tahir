#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <hbrs_msgs/PlanarPolygon.h>
#include "aliases.h"

/** Convert from PCL PlanarPolygon to ROS message. */
void convertPlanarPolygon(const PlanarPolygon& pcl_polygon, hbrs_msgs::PlanarPolygon& ros_polygon)
{
  for (int i = 0; i < 4; ++i)
  {
    ros_polygon.coefficients[i] = pcl_polygon.getCoefficients()[i];
  }
  for (const auto& point : pcl_polygon.getContour())
  {
    geometry_msgs::Point32 pt;
    pt.x = point.x;
    pt.y = point.y;
    pt.z = point.z;
    ros_polygon.contour.push_back(pt);
  }
}

/** Convert from ROS message to PCL PlanarPolygon. */
void convertPlanarPolygon(const hbrs_msgs::PlanarPolygon& ros_polygon, PlanarPolygon& pcl_polygon)
{
  PointCloud contour;
  Eigen::Vector4f coefficients(ros_polygon.coefficients.elems);
  for (const auto& point : ros_polygon.contour)
  {
    PointT pt;
    pt.x = point.x;
    pt.y = point.y;
    pt.z = point.z;
    contour.points.push_back(pt);
  }
  pcl_polygon.setContour(contour);
  pcl_polygon.setCoefficients(coefficients);
}

#endif /* HELPERS_HPP */

