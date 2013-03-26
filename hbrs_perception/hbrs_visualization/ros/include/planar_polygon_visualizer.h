#ifndef PLANAR_POLYGON_VISUALIZER_H
#define PLANAR_POLYGON_VISUALIZER_H

#include <string>

#include <ros/ros.h>
#include <visualization_msgs/Marker.h>

#include <pcl16/point_cloud.h>
#include <pcl16/geometry/planar_polygon.h>

#include "color.h"

namespace hbrs
{

namespace visualization
{

class PlanarPolygonVisualizer
{

public:

  PlanarPolygonVisualizer(const std::string& topic_name,
                          Color color,
                          bool check_subscribers = true);

  template<typename PointT>
  void publish(const pcl16::PlanarPolygon<PointT>& polygon,
               const std::string& frame_id);

  /** Fill the fields of the marker object so that it visualizes the provided
    * vector of points by drawing a polyline through them. */
  template<typename PointT>
  void buildPolygonMarker(const typename pcl16::PointCloud<PointT>::VectorType& points,
                          visualization_msgs::Marker& marker,
                          const std::string& frame_id,
                          int id = 1);

private:

  ros::Publisher marker_publisher_;

  const std::string frame_id_;
  const Color color_;
  bool check_subscribers_;

};

}

}

#include "impl/planar_polygon_visualizer.hpp"

#endif /* PLANAR_POLYGON_VISUALIZER_H */

