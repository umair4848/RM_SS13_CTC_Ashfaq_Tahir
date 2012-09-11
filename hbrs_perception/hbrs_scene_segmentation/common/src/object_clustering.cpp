#include <boost/make_shared.hpp>

#include "object_clustering.h"

ObjectClustering::ObjectClustering()
{
  ece_.setSearchMethod(boost::make_shared<pcl::search::KdTree<PointT>>());
  ece_.setClusterTolerance(0.01);
  ece_.setMinClusterSize(15);
  ece_.setMaxClusterSize(5000);
}

ObjectClustering::ObjectClustering(double object_cluster_tolerance,
                                   int object_cluster_min_size,
                                   int object_cluster_max_size)
{
  ece_.setSearchMethod(boost::make_shared<pcl::search::KdTree<PointT>>());
  ece_.setClusterTolerance(object_cluster_tolerance);
  ece_.setMinClusterSize(object_cluster_min_size);
  ece_.setMaxClusterSize(object_cluster_max_size);
}

void ObjectClustering::cluster(std::vector<PointCloud::Ptr>& clusters)
{
  std::vector<pcl::PointIndices> clusters_indices;
  ece_.setInputCloud(input_);
  ece_.extract(clusters_indices);

  for (const pcl::PointIndices& cluster_indices : clusters_indices)
  {
    PointCloud::Ptr cluster_cloud(new PointCloud);
    pcl::copyPointCloud(*input_, cluster_indices, *cluster_cloud);
    clusters.push_back(cluster_cloud);
  }

  PCL_INFO("[ObjectClustering::cluster] Found %zu clusters.\n", clusters_indices.size());
}

