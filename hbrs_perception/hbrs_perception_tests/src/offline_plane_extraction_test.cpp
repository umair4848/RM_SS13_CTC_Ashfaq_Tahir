#include <ros/ros.h>
#include <ros/console.h>

#include "reconfigurable_plane_extraction.hpp"
#include "offline_test_base.hpp"

class TestNode : public OfflineTestBase
{

public:

  TestNode()
  : OfflineTestBase()
  , rpe_(std::bind(&TestNode::process, this))
  {
  }

protected:

  void process()
  {
    PlanarPolygonVector planar_polygons;
    rpe_.plane_extraction_ptr->setInputCloud(cloud_);
    MEASURE_RUNTIME(rpe_.plane_extraction_ptr->extract(planar_polygons), "Plane extraction");

    std::cout << "Number of planes extracted: " << planar_polygons.size() << std::endl;
    for (const auto& planar_polygon : planar_polygons)
    {
      std::cout << "---\n";
      std::cout << "Points in contour: " << planar_polygon.getContour().size() << std::endl;
      std::cout << "Plane coefficients:\n" << planar_polygon.getCoefficients() << std::endl;
    }

    viewer_.removeAllPointClouds(0);
    viewer_.removeAllShapes(0);

    displayCloud();

    if (rpe_.plane_normal_ptr)
      displayNormal(*rpe_.plane_normal_ptr);

    displayPlanarPolygons(planar_polygons);
  }

  ReconfigurablePlaneExtraction rpe_;

};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "plane_extraction");

  if (argc != 2 && argc != 4)
  {
    ROS_ERROR("Usage: %s <filename.pcd> [min_z max_z]", argv[0]);
    return 1;
  }

  TestNode tn;
  tn.loadCloud(argv[1]);

  if (argc == 4)
    tn.applyPassThroughFilter(argv[2], argv[3]);

  tn.run();

  return 0;
}
