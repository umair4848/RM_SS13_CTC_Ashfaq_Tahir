#ifndef TEST_BASE_HPP_
#define TEST_BASE_HPP_

#include <boost/lexical_cast.hpp>

#include <ros/ros.h>
#include <pcl/visualization/pcl_visualizer.h>

#define MEASURE_RUNTIME(function, description) \
        { \
          double ts1 = pcl::getTime(); \
          (function); \
          double ts2 = pcl::getTime(); \
          std::cout << (description) << " took " << ts2 - ts1 << " seconds." << std::endl; \
        }

/** Base class for perception test programs.
  * Provides PCLVisualizer initialization, infinite superloop (@ref run()),
  * and several helper functions to display different objects. */
class TestBase
{

public:

  TestBase()
  : viewer_("Viewer")
  {
    viewer_.setBackgroundColor(0, 0, 0);
    viewer_.addCoordinateSystem(1.0);
    viewer_.initCameraParameters();
  }

  virtual ~TestBase() { }

  void run()
  {
    process();
    while (!viewer_.wasStopped() && !ros::isShuttingDown())
    {
      viewer_.spinOnce(100);
      ros::spinOnce();
    }
  }

protected:

  virtual void process() = 0;

  void displayNormal(Eigen::Vector3f normal, Eigen::Vector3f start = Eigen::Vector3f::Zero())
  {
    pcl::PointXYZ pt1 = pcl::PointXYZ(start[0], start[1], start[2]);
    pcl::PointXYZ pt2 = pcl::PointXYZ(start[0] + (0.5 * normal[0]),
                                      start[1] + (0.5 * normal[1]),
                                      start[2] + (0.5 * normal[2]));
    viewer_.addArrow(pt2, pt1, 1.0, 0, 0, false, "normal");
  }

  /** For the given PlanarPolygon draw a polyline through its points and also
    * display the points themselves.
    *
    * Polygon is drawn in red by defalut and has an identifier "polygon0". */
  void displayPlanarPolygon(const PlanarPolygon& polygon, unsigned char r = 255, unsigned char g = 0, unsigned char b = 0, int id = 0)
  {
    PointCloud::Ptr contour(new PointCloud);
    contour->points = polygon.getContour();
    if (!contour->points.size())
      return;
    std::string polygon_name = "polygon" + boost::lexical_cast<std::string>(id);
    std::string points_name = polygon_name + "_points";
    viewer_.addPolygon<PointT>(contour, r, g, b, polygon_name);
    pcl::visualization::PointCloudColorHandlerCustom<PointT> color(contour, r, g, b);
    viewer_.addPointCloud(contour, color, points_name);
    viewer_.setPointCloudRenderingProperties(pcl::visualization::PCL_VISUALIZER_POINT_SIZE, 6, points_name);
  }

  void displayPlanarPolygons(const PlanarPolygonVector& polygons)
  {
    for (size_t i = 0; i < polygons.size(); i++)
    {
      unsigned char r = 255 - i * 5;
      unsigned char b = 0   + i * 5;
      displayPlanarPolygon(polygons[i], r, 0, b, i);
    }
  }

  pcl::visualization::PCLVisualizer viewer_;

};

#endif

