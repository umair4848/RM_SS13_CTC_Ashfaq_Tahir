#include <pcl/octree/octree_impl.h>

#include "cloud_accumulation.h"

CloudAccumulation::CloudAccumulation(double resolution)
: resolution_(resolution)
{
  reset();
}

void CloudAccumulation::addCloud(const PointCloud::ConstPtr& cloud)
{
  if (cloud_count_ == 0)
  {
    octree_->setInputCloud(cloud);
    octree_->addPointsFromInputCloud();
  }
  else
  {
    for (const auto& point: cloud->points)
      if ((point.x == point.x) && (point.y == point.y) && (point.z == point.z)) // NaN check
        octree_->setOccupiedVoxelAtPoint(point);
  }
  cloud_count_++;
}

void CloudAccumulation::getAccumulatedCloud(PointCloud& cloud)
{
  octree_->getOccupiedVoxelCenters(cloud.points);
  cloud.width = cloud.points.size();
  cloud.height = 1;
}

void CloudAccumulation::reset()
{
  octree_ = OctreeUPtr(new Octree(resolution_));
  cloud_count_ = 0;
}

