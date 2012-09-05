#ifndef CLOUD_ACCUMULATION_H
#define CLOUD_ACCUMULATION_H

#include <pcl/octree/octree_pointcloud_occupancy.h>

#include "aliases.h"

/** This class accumulates input point clouds in the occupancy octree with a
  * given spatial resolution. */
class CloudAccumulation
{

public:

  explicit CloudAccumulation(double resolution = 0.0025);

  void addCloud(const PointCloud::ConstPtr& cloud);

  void getAccumulatedCloud(PointCloud& cloud);

private:

  pcl::octree::OctreePointCloudOccupancy<PointT> octree_;

  int num_clouds_;

};

#endif /* CLOUD_ACCUMULATION_H */
