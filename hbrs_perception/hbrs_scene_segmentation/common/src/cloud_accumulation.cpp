#include <pcl/octree/octree_impl.h>

#include "cloud_accumulation.h"

CloudAccumulation::CloudAccumulation(double resolution)
: octree_(resolution)
, num_clouds_(0)
{
}

void CloudAccumulation::addCloud(const PointCloud::ConstPtr& cloud)
{
  if (num_clouds_ == 0)
  {
    octree_.setInputCloud(cloud);
    octree_.addPointsFromInputCloud();
  }
  else
  {
    for (const auto& point: cloud->points)
      if ((point.x == point.x) && (point.y == point.y) && (point.z == point.z)) // NaN check
        octree_.setOccupiedVoxelAtPoint(point);
  }
  num_clouds_++;
}

void CloudAccumulation::getAccumulatedCloud(PointCloud& cloud)
{
  octree_.getOccupiedVoxelCenters(cloud.points);
  cloud.width = cloud.points.size();
  cloud.height = 1;
}

