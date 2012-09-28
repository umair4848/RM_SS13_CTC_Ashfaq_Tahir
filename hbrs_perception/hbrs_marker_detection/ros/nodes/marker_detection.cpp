/*
 *  Multi Marker Pose Estimation using ARToolkit
 *  Copyright (C) 2010, CCNY Robotics Lab, 2011 ILS Robotics Lab
 *  Ivan Dryanovski <ivan.dryanovski@gmail.com>
 *  William Morris <morris@ee.ccny.cuny.edu>
 *  Gautier Dumonteil <gautier.dumonteil@gmail.com>
 *  http://robotics.ccny.cuny.edu
 *
 *  Michael Ferguson <ferguson@cs.albany.edu>
 *  http://robotics.ils.albany.edu
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *
 *  modified on: 22.09.2012
 *  modified by: Frederik Hegger
 */

#include <ros/ros.h>
#include <std_srvs/Empty.h>
#include <string.h>
#include <stdarg.h>

#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <AR/arMulti.h>

#include <ros/ros.h>
#include <ros/package.h>
#include <ros/console.h>
#include <ros/topic.h>
#include <geometry_msgs/TransformStamped.h>
#include <tf/transform_broadcaster.h>
#include <visualization_msgs/Marker.h>

#include <sensor_msgs/PointCloud2.h>
#include <pcl/ros/conversions.h>
#include <pcl/point_types.h>
#include <pcl/registration/icp.h>
#include <pcl/registration/registration.h>

#include <opencv/cv.h>
#include <cv_bridge/CvBridge.h>

#include <ar_pose/ARMarkers.h>
#include <ar_pose/ARMarker.h>

#include <hbrs_srvs/GetObjects.h>
#include <hbrs_msgs/ObjectList.h>
#include <hbrs_msgs/Object.h>

#include "ar_object.h"

const double AR_TO_ROS = 0.001;
typedef pcl::PointCloud<pcl::PointXYZRGB> PointCloud;

class ARMarkerDetection
{
public:
	ARMarkerDetection(ros::NodeHandle & n);
	~ARMarkerDetection(void);
	bool detectMarkerCallback(hbrs_srvs::GetObjects::Request &req, hbrs_srvs::GetObjects::Response &res);
	void pointCloudCallback(sensor_msgs::PointCloud2::ConstPtr msg);


private:
	void arInit();
	tf::Transform tfFromEigen(Eigen::Matrix4f trans);
	pcl::PointXYZRGB makeRGBPoint(float x, float y, float z);

	ros::NodeHandle n_;
	ros::Publisher arMarkerPub_;

	sensor_msgs::CvBridge bridge_;

	/* create a Service Server*/
	ros::ServiceServer srvDetectMarker_;

	ros::Subscriber sub_cloud_;
	bool received_new_data_;
	sensor_msgs::PointCloud2::ConstPtr input_cloud_;

	// **** for visualisation in rviz
	ros::Publisher rvizMarkerPub_;
	visualization_msgs::Marker rvizMarker_;

	// **** parameters
	ARParam cam_param_; // Camera Calibration Parameters
	ARMultiMarkerInfoT *config; // AR Marker Info
	ar_object::ObjectData_T * object;
	int objectnum;
	char pattern_filename_[FILENAME_MAX];
	char data_directory_[FILENAME_MAX];

	ar_pose::ARMarkers arPoseMarkers_;
	int threshold_;
	bool getCamInfo_;
	bool publishTf_;
	bool publishVisualMarkers_;
	CvSize sz_;
	IplImage *capture_;
	bool configured_;
};


ARMarkerDetection::ARMarkerDetection(ros::NodeHandle & n) : n_(n), configured_(false)
{
	std::string path;
	std::string package_path = ros::package::getPath(ROS_PACKAGE_NAME);
	ros::NodeHandle n_param("~");
	XmlRpc::XmlRpcValue xml_marker_center;

	srvDetectMarker_ = n_.advertiseService("detect_marker", &ARMarkerDetection::detectMarkerCallback, this);

	// **** get parameters
	if (!n_param.getParam("publish_visual_markers", publishVisualMarkers_))
		publishVisualMarkers_ = true;
	ROS_INFO("\tPublish visual markers: %d", publishVisualMarkers_);

	if (!n_param.getParam("threshold", threshold_))
		threshold_ = 100;
	ROS_INFO("\tThreshold: %d", threshold_);

	if (!n_param.getParam("marker_pattern_list", path))
		sprintf(pattern_filename_, "%s/data/objects_kinect", package_path.c_str());
	else
		sprintf(pattern_filename_, "%s", path.c_str());
	ROS_INFO("Marker Pattern Filename: %s", pattern_filename_);

	if (!n_param.getParam("marker_data_directory", path))
		sprintf(data_directory_, "%s", package_path.c_str());
	else
		sprintf(data_directory_, "%s", path.c_str());
	ROS_INFO("Marker Data Directory: %s", data_directory_);

	// **** advertise
	arMarkerPub_ = n_.advertise < ar_pose::ARMarkers > ("ar_pose_markers", 0);
	if (publishVisualMarkers_)
		rvizMarkerPub_ = n_.advertise<visualization_msgs::Marker> ("visualization_marker", 0);

	received_new_data_ = false;
}

ARMarkerDetection::~ARMarkerDetection(void)
{
	arVideoCapStop();
	arVideoClose();
}

void ARMarkerDetection::pointCloudCallback(sensor_msgs::PointCloud2::ConstPtr msg)
{
	input_cloud_ = msg;
	received_new_data_ = true;
}

/* 
 * Setup artoolkit
 */
void ARMarkerDetection::arInit()
{
	arInitCparam(&cam_param_);
	ROS_INFO("*** Camera Parameter ***");
	arParamDisp(&cam_param_);

	// load in the object data - trained markers and associated bitmap files
	if ((object = ar_object::read_ObjData(pattern_filename_, data_directory_, &objectnum)) == NULL)
		ROS_BREAK();
	ROS_DEBUG("Objectfile num = %d", objectnum);

	sz_ = cvSize(cam_param_.xsize, cam_param_.ysize);
	capture_ = cvCreateImage(sz_, IPL_DEPTH_8U, 4);
	configured_ = true;
}


/*
 * Service Callback
 */
bool ARMarkerDetection::detectMarkerCallback(hbrs_srvs::GetObjects::Request &req, hbrs_srvs::GetObjects::Response &res)
{
	hbrs_msgs::ObjectList marker_obj_list;

	sensor_msgs::PointCloud2::ConstPtr msg;
	sub_cloud_ = n_.subscribe("/camera/rgb/points/", 1, &ARMarkerDetection::pointCloudCallback, this);

	for(unsigned int i=0; i < 10; ++i)
	{
		while(!received_new_data_)
			ros::spinOnce();

		msg = input_cloud_;

		sensor_msgs::ImagePtr image_msg(new sensor_msgs::Image);
		ARUint8 *dataPtr;
		ARMarkerInfo *marker_info;
		int marker_num;
		int i, k, j;

		/* do we need to initialize? */
		if (!configured_)
		{
			if (msg->width == 0 || msg->height == 0)
			{
				ROS_ERROR("Deformed cloud! Size = %d, %d.", msg->width, msg->height);
				return false;
			}

			cam_param_.xsize = msg->width;
			cam_param_.ysize = msg->height;

			cam_param_.dist_factor[0] = msg->width / 2; // x0 = cX from openCV calibration
			cam_param_.dist_factor[1] = msg->height / 2; // y0 = cY from openCV calibration
			cam_param_.dist_factor[2] = 0; // f = -100*k1 from CV. Note, we had to do mm^2 to m^2, hence 10^8->10^2
			cam_param_.dist_factor[3] = 1.0; // scale factor, should probably be >1, but who cares...

			arInit();
		}

		/* convert cloud to PCL */
		PointCloud cloud;
		pcl::fromROSMsg(*msg, cloud);

		/* get an OpenCV image from the cloud */
		pcl::toROSMsg(cloud, *image_msg);
		try
		{
			capture_ = bridge_.imgMsgToCv(image_msg, "bgr8");
		} catch (sensor_msgs::CvBridgeException & e)
		{
			ROS_ERROR("Could not convert from '%s' to 'bgr8'.",
					image_msg->encoding.c_str());
		}
		dataPtr = (ARUint8 *) capture_->imageData;

		/* detect the markers in the video frame */
		if (arDetectMarkerLite(dataPtr, threshold_, &marker_info, &marker_num) < 0)
		{
			argCleanup();
			return false;
		}

		arPoseMarkers_.markers.clear();
		/* check for known patterns */
		for (i = 0; i < objectnum; i++)
		{
			k = -1;
			for (j = 0; j < marker_num; j++)
			{
				if (object[i].id == marker_info[j].id)
				{
					if (k == -1)
						k = j;
					else // make sure you have the best pattern (highest confidence factor)
					if (marker_info[k].cf < marker_info[j].cf)
						k = j;
				}
			}

			if (k == -1)
			{
				object[i].visible = 0;
				continue;
			}

			/* create a cloud for marker corners */
			int d = marker_info[k].dir;
			PointCloud marker;
			marker.push_back(
					cloud.at((int) marker_info[k].vertex[(4 - d) % 4][0],
							(int) marker_info[k].vertex[(4 - d) % 4][1])); // upper left
			marker.push_back(
					cloud.at((int) marker_info[k].vertex[(5 - d) % 4][0],
							(int) marker_info[k].vertex[(5 - d) % 4][1])); // upper right
			marker.push_back(
					cloud.at((int) marker_info[k].vertex[(6 - d) % 4][0],
							(int) marker_info[k].vertex[(6 - d) % 4][1])); // lower right
			marker.push_back(
					cloud.at((int) marker_info[k].vertex[(7 - d) % 4][0],
							(int) marker_info[k].vertex[(7 - d) % 4][1]));

			/* create an ideal cloud */
			double w = object[i].marker_width;
			PointCloud ideal;
			ideal.push_back(makeRGBPoint(-w / 2, w / 2, 0));
			ideal.push_back(makeRGBPoint(w / 2, w / 2, 0));
			ideal.push_back(makeRGBPoint(w / 2, -w / 2, 0));
			ideal.push_back(makeRGBPoint(-w / 2, -w / 2, 0));

			/* get transformation */
			Eigen::Matrix4f t;
			pcl::estimateRigidTransformationSVD(marker, ideal, t);

			/* get final transformation */
			tf::Transform transform = tfFromEigen(t.inverse());

			// any(transform == nan)
			btMatrix3x3 m = transform.getBasis();
			btVector3 p = transform.getOrigin();
			bool invalid = false;
			for (int l = 0; l < 3; l++)
				for (int n = 0; n < 3; n++)
					invalid = (invalid || isnan(m[l][n]) || fabs(m[l][n]) > 1.0);

			for (int l = 0; l < 3; l++)
				invalid = (invalid || isnan(p[l]));

			if (invalid)
				continue;

			/* publish the marker */
			ar_pose::ARMarker ar_pose_marker;
			ar_pose_marker.header.frame_id = msg->header.frame_id;
			ar_pose_marker.header.stamp = msg->header.stamp;
			ar_pose_marker.id = object[i].id;

			ar_pose_marker.pose.pose.position.x = transform.getOrigin().getX();
			ar_pose_marker.pose.pose.position.y = transform.getOrigin().getY();
			ar_pose_marker.pose.pose.position.z = transform.getOrigin().getZ();

			ar_pose_marker.pose.pose.orientation.x =
					transform.getRotation().getAxis().getX();
			ar_pose_marker.pose.pose.orientation.y =
					transform.getRotation().getAxis().getY();
			ar_pose_marker.pose.pose.orientation.z =
					transform.getRotation().getAxis().getZ();
			ar_pose_marker.pose.pose.orientation.w = transform.getRotation().getW();

			ar_pose_marker.confidence = marker_info->cf;
			arPoseMarkers_.markers.push_back(ar_pose_marker);

			// fill hbrs msgs
			hbrs_msgs::Object marker_obj;

			marker_obj.pose.pose = ar_pose_marker.pose.pose;
			marker_obj.pose.header = ar_pose_marker.header;
			marker_obj.name = object[i].name;
			marker_obj.category = "ar_marker";

			marker_obj_list.objects.push_back(marker_obj);


			/* publish visual marker */
			if (publishVisualMarkers_)
			{
				btVector3 markerOrigin(0, 0, 0.25 * object[i].marker_width * AR_TO_ROS);
				btTransform m(btQuaternion::getIdentity(), markerOrigin);
				btTransform markerPose = transform * m; // marker pose in the camera frame

				tf::poseTFToMsg(markerPose, rvizMarker_.pose);

				rvizMarker_.header.frame_id = msg->header.frame_id;
				rvizMarker_.header.stamp = msg->header.stamp;
				rvizMarker_.id = object[i].id;

				rvizMarker_.scale.x = 1.0 * object[i].marker_width * AR_TO_ROS;
				rvizMarker_.scale.y = 1.0 * object[i].marker_width * AR_TO_ROS;
				rvizMarker_.scale.z = 0.5 * object[i].marker_width * AR_TO_ROS;
				rvizMarker_.ns = "basic_shapes";
				rvizMarker_.type = visualization_msgs::Marker::CUBE;
				rvizMarker_.action = visualization_msgs::Marker::ADD;
				switch (i)
				{
				case 0:
					rvizMarker_.color.r = 0.0f;
					rvizMarker_.color.g = 0.0f;
					rvizMarker_.color.b = 1.0f;
					rvizMarker_.color.a = 1.0;
					break;
				case 1:
					rvizMarker_.color.r = 1.0f;
					rvizMarker_.color.g = 0.0f;
					rvizMarker_.color.b = 0.0f;
					rvizMarker_.color.a = 1.0;
					break;
				default:
					rvizMarker_.color.r = 0.0f;
					rvizMarker_.color.g = 1.0f;
					rvizMarker_.color.b = 0.0f;
					rvizMarker_.color.a = 1.0;
				}
				rvizMarker_.lifetime = ros::Duration();

				rvizMarkerPub_.publish(rvizMarker_);
				ROS_DEBUG("Published visual marker");
			}
		}

		arMarkerPub_.publish(arPoseMarkers_);
		ROS_DEBUG("Published ar_multi markers");

		received_new_data_ = false;

		if(!marker_obj_list.objects.empty())
			break;
	}

	sub_cloud_.shutdown();

	res.objects = marker_obj_list.objects;

	return true;
}

tf::Transform ARMarkerDetection::tfFromEigen(Eigen::Matrix4f trans)
{
	btMatrix3x3 btm;
	btm.setValue(trans(0, 0), trans(0, 1), trans(0, 2), trans(1, 0), trans(1, 1), trans(1, 2), trans(2, 0), trans(2, 1), trans(2, 2));

	btTransform ret;
	ret.setOrigin(btVector3(trans(0, 3), trans(1, 3), trans(2, 3)));
	ret.setBasis(btm);

	return ret;
}

pcl::PointXYZRGB ARMarkerDetection::makeRGBPoint(float x, float y, float z)
{
	pcl::PointXYZRGB p;
	p.x = x;
	p.y = y;
	p.z = z;
	return p;
}

int main(int argc, char **argv)
{
	/* init ROS node with a name and a node handle*/
	ros::init(argc, argv, "raw_marker_detection");
	ros::NodeHandle nh;

	ARMarkerDetection marker_detection(nh);
	/* create a loop rate to let your node run only with maximum frequency, here 2Hz */
	ros::Rate loop_rate(5);

	while (ros::ok())
	{
		/* process topic callbacks and service request */
		ros::spinOnce();

		/* wait to ensure that is not running than the predefined loop rate */
		loop_rate.sleep();
	}

	return 0;
}

