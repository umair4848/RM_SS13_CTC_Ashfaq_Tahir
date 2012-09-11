#ifndef OBJECT_CLUSTERING_H
#define OBJECT_CLUSTERING_H

#include <pcl/segmentation/extract_clusters.h>

#include "aliases.h"

class ObjectClustering
{

public:

  ObjectClustering();

  ObjectClustering(double object_cluster_tolerance,
                   int object_cluster_min_size,
                   int object_cluster_max_size);

  ~ObjectClustering()
  {
    input_.reset();
  }

  void cluster(std::vector<PointCloud::Ptr>& clusters);

  inline void setInputCloud(const PointCloud::ConstPtr& cloud)
  {
    input_ = cloud;
  }

  inline PointCloud::ConstPtr getInputCloud() const
  {
    return input_;
  }

private:

  PointCloud::ConstPtr input_;

  pcl::EuclideanClusterExtraction<PointT> ece_;

};

#endif /* OBJECT_CLUSTERING_H */

