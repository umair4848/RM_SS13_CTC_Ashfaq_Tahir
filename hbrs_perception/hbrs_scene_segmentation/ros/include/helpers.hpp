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

double computePlanarPolygonArea(const PlanarPolygon& polygon)
{
  const auto& normal = polygon.getCoefficients();
  const auto& points = polygon.getContour();

  // Find axis with largest normal component and project onto perpendicular plane
  int k0, k1, k2;
  k0 = (std::fabs(normal[0]) > std::fabs(normal[1])) ? 0 : 1;
  k0 = (std::fabs(normal[k0]) > std::fabs(normal[2])) ? k0 : 2;
  k1 = (k0 + 1) % 3;
  k2 = (k0 + 2) % 3;

  double area = 0;
  for (size_t i = 0; i < points.size(); i++)
  {
    size_t j = (i + 1) % points.size();
    float p1[3] = { points[i].x, points[i].y, points[i].z };
    float p2[3] = { points[j].x, points[j].y, points[j].z };
    area += p1[k1] * p2[k2] - p1[k2] * p2[k1];
  }

  return std::fabs(area) / (2 * std::fabs(normal[k0]));
}

#endif /* HELPERS_HPP */

