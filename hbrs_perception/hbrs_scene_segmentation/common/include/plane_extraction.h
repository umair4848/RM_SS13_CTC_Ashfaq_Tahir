#ifndef PLANE_EXTRACTION_H_
#define PLANE_EXTRACTION_H_

#include <pcl/features/integral_image_normal.h>
#include <pcl/filters/project_inliers.h>
#include <pcl/segmentation/organized_multi_plane_segmentation.h>

#include "aliases.h"

class PlaneExtraction
{

public:

  typedef std::shared_ptr<PlaneExtraction> Ptr;
  typedef std::unique_ptr<PlaneExtraction> UPtr;

  /** Default constructor will setup all involved PCL classes with sensible
    * default parameters. */
  PlaneExtraction();

  /** This constructor allows to set parameters of all involved PCL classs. */
  PlaneExtraction(double normal_max_depth_change_factor,
                  double normal_smoothing_size,
                  unsigned int min_inliers,
                  double angular_threshold,
                  double distance_threshold,
                  double maximum_curvature,
                  double refinement_threshold,
                  double refinement_depth_dependent);

  void extract(PlanarPolygonVector& planar_polygons);

  inline void setInputCloud(const PointCloud::ConstPtr &cloud)
  {
    input_ = cloud;
  }

  inline PointCloud::ConstPtr getInputCloud() const
  {
    return input_;
  }

  virtual ~PlaneExtraction()
  {
    input_.reset();
  }

private:

  PointCloud::ConstPtr input_;
  pcl::IntegralImageNormalEstimation<PointT, PointNT> ne_;
  pcl::OrganizedMultiPlaneSegmentation<PointT, PointNT, PointLT> mps_;
  pcl::ProjectInliers<PointT> pi_;

};

#endif
