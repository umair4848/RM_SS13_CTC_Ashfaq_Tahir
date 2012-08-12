#include <pcl/ModelCoefficients.h>
#include <pcl/common/angles.h>
#include <pcl/console/print.h>
#include <pcl/surface/convex_hull.h>

#include "plane_extraction.h"

PlaneExtraction::PlaneExtraction()
{
  // Setup IntegralImageNormalEstimation
  ne_.setNormalEstimationMethod(ne_.COVARIANCE_MATRIX);
  ne_.setMaxDepthChangeFactor(0.02);
  ne_.setNormalSmoothingSize(10.0);
  // Setup OrganizedMultiPlaneSegmentation
  mps_.setMinInliers(1000);
  mps_.setAngularThreshold(pcl::deg2rad(3.0));
  mps_.setDistanceThreshold(0.02);
  mps_.setMaximumCurvature(0.005);
  // Setup ProjectInliers
  pi_.setModelType(pcl::SACMODEL_NORMAL_PLANE);
}

PlaneExtraction::PlaneExtraction(double normal_max_depth_change_factor,
                                 double normal_smoothing_size,
                                 unsigned int min_inliers,
                                 double angular_threshold,
                                 double distance_threshold,
                                 double maximum_curvature,
                                 double refinement_threshold,
                                 double refinement_depth_dependent)
{
  // Setup IntegralImageNormalEstimation
  ne_.setNormalEstimationMethod(ne_.COVARIANCE_MATRIX);
  ne_.setMaxDepthChangeFactor(normal_max_depth_change_factor);
  ne_.setNormalSmoothingSize(normal_smoothing_size);
  // Setup OrganizedMultiPlaneSegmentation
  mps_.setMinInliers(min_inliers);
  mps_.setAngularThreshold(angular_threshold);
  mps_.setDistanceThreshold(distance_threshold);
  mps_.setMaximumCurvature(maximum_curvature);
  pcl::PlaneRefinementComparator<PointT, PointNT, PointLT>::Ptr refinement_compare(new pcl::PlaneRefinementComparator<PointT, PointNT, PointLT>());
  refinement_compare->setDistanceThreshold(refinement_threshold, refinement_depth_dependent);
  mps_.setRefinementComparator(refinement_compare);
  // Setup ProjectInliers
  pi_.setModelType(pcl::SACMODEL_NORMAL_PLANE);
}

void PlaneExtraction::extract(PlanarPolygonVector& planar_polygons)
{
  planar_polygons.clear();

  // Step 1: perform multiplanar segmentation
  PointCloudN::Ptr normals(new PointCloudN);
  std::vector<pcl::PlanarRegion<PointT>, Eigen::aligned_allocator<pcl::PlanarRegion<PointT>>> regions;
  std::vector<pcl::ModelCoefficients> model_coefficients;
  std::vector<pcl::PointIndices> inlier_indices;
  PointCloudL::Ptr labels(new PointCloudL);
  std::vector<pcl::PointIndices> label_indices;
  std::vector<pcl::PointIndices> boundary_indices;
  // Estimate normals
  ne_.setInputCloud(input_);
  ne_.compute(*normals);
  // Run multiplane segmentation with refinement
  mps_.setInputCloud(input_);
  mps_.setInputNormals(normals);
  mps_.segmentAndRefine(regions, model_coefficients, inlier_indices, labels, label_indices, boundary_indices);
  if (regions.size() == 0)
  {
    ROS_WARN("[PlaneExtraction::extract] No planar regions found!");
    return;
  }

  // Step 2: compute convex hull for each found region that satisfies the
  // constraints (if set), and build planar polygons.
  pi_.setInputCloud(input_);
  for (size_t i = 0; i < regions.size(); i++)
  {
    if (apply_angular_constraints_)
    {
      Eigen::Vector3f region_normal = regions.at(i).getCoefficients().head<3>();
      if (region_normal.dot(plane_normal_) < angular_threshold_)
        continue;
    }
    if (apply_distance_constraints_)
    {
      double region_distance = regions.at(i).getCoefficients()(3);
      if (std::abs(region_distance - distance_) > distance_threshold_)
        continue;
    }
    PointCloud::Ptr region_cloud(new PointCloud);
    PointCloud::Ptr region_hull(new PointCloud);
    // Project boundary points onto the plane
    pi_.setIndices(boost::make_shared<pcl::PointIndices>(boundary_indices.at(i)));
    pi_.setModelCoefficients(boost::make_shared<pcl::ModelCoefficients>(model_coefficients.at(i)));
    pi_.filter(*region_cloud);
    // Compute convex hull around boundary points
    pcl::ConvexHull<PointT> convex_hull;
    convex_hull.setInputCloud(region_cloud);
    convex_hull.reconstruct(*region_hull);
    // Create and push planar polygon
    planar_polygons.push_back(PlanarPolygon(region_hull->points, regions.at(i).getCoefficients()));
  }
}

