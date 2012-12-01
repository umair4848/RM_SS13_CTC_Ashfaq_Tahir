/*
 * ros_arm_cartesian_control.cpp
 *
 *  Created on: Nov 29, 2012
 *      Author: matthias
 */

#include "ros_urdf_loader.h"
#include "arm_cartesian_control.h"

#include <sensor_msgs/JointState.h>
#include <kdl/kdl.hpp>
#include <geometry_msgs/Twist.h>
#include <ros/ros.h>
#include <kdl/chainiksolvervel_wdls.hpp>
#include <brics_actuator/JointVelocities.h>
#include <urdf_interface/joint.h>

KDL::Chain arm_chain;
std::vector<boost::shared_ptr<urdf::JointLimits> > joint_limits;

KDL::JntArray joint_positions;

KDL::Twist targetVelocity;

KDL::ChainIkSolverVel* ik_solver;
Eigen::MatrixXd weight_ts;
Eigen::MatrixXd weight_js;

ros::Publisher cmd_vel_publisher;

bool active = false;
ros::Time t_last_command;

brics_actuator::JointVelocities jointMsg;


void jointstateCallback(sensor_msgs::JointStateConstPtr joints) {

	for (unsigned i = 0; i < joints->position.size(); i++) {

		const char* joint_uri = joints->name[i].c_str();

		for (unsigned int j = 0; j < arm_chain.getNrOfJoints(); j++) {
			const char* chainjoint =
					arm_chain.getSegment(j).getJoint().getName().c_str();

			if (chainjoint != 0 && strcmp(chainjoint, joint_uri) == 0) {
				joint_positions.data[j] = joints->position[i];
			}
		}
	}
}

void ccCallback(geometry_msgs::TwistConstPtr desiredVelocity) {

	targetVelocity.vel.data[0] = desiredVelocity->linear.x;
	targetVelocity.vel.data[1] = desiredVelocity->linear.y;
	targetVelocity.vel.data[2] = desiredVelocity->linear.z;

	targetVelocity.rot.data[0] = desiredVelocity->angular.x;
	targetVelocity.rot.data[1] = desiredVelocity->angular.y;
	targetVelocity.rot.data[2] = desiredVelocity->angular.z;

	t_last_command = ros::Time::now();
	active = true;
}


void init_ik_solver() {

	if(ik_solver != 0) {
		return;
	}

	ik_solver = new KDL::ChainIkSolverVel_wdls(arm_chain);
	weight_ts.resize(6, 6);
	weight_ts.setIdentity();

	//weight_ts(0, 0) = 1;
	//weight_ts(1, 1) = 1;
	//weight_ts(2, 2) = 10;
	//weight_ts(3, 3) = 0.0001;
	//weight_ts(4, 4) = 0.0001;
	//weight_ts(5, 5) = 0.0001;
	//((KDL::ChainIkSolverVel_wdls*) ik_solver)->setWeightTS(weight_ts);

	weight_js = (Eigen::MatrixXd::Identity(arm_chain.getNrOfJoints(),
			arm_chain.getNrOfJoints()));
	//weight_js(0, 0) = 0.5;
	//weight_js(1,1) = 1;
	//weight_js(2,2) = 1;
	//weight_js(3,3) = 1;
	//weight_js(4,4) = 0.1;
	//((KDL::ChainIkSolverVel_wdls*) ik_solver)->setWeightJS(weight_js);
}

void init_joint_msgs() {
	jointMsg.velocities.resize(arm_chain.getNrOfJoints());
	for (unsigned int i = 0; i < arm_chain.getNrOfSegments(); i++) {
		jointMsg.velocities[i].joint_uri =
				arm_chain.getSegment(i).getJoint().getName();
		jointMsg.velocities[i].unit = "s^-1 rad";
	}
}

void publishJointVelocities(KDL::JntArrayVel& joint_velocities) {


	for (unsigned int i=0; i<joint_velocities.qdot.rows(); i++) {
		jointMsg.velocities[i].value = joint_velocities.qdot(i);
	}

	cmd_vel_publisher.publish(jointMsg);
}


void stopMotion() {

	for (unsigned int i = 0; i < jointMsg.velocities.size(); i++) {
		jointMsg.velocities[i].value = 0.0;

	}
	cmd_vel_publisher.publish(jointMsg);
}


bool watchdog() {

	double watchdog_time = 0.3;
	if (active==false) {
		return false;
	}

	ros::Time now = ros::Time::now();

	ros::Duration time = (now - t_last_command);

	if ( time > ros::Duration(watchdog_time) ) {
		active = false;
		stopMotion();
		return false;
	}

	return true;
}


int main(int argc, char **argv) {
	ros::init(argc, argv, "arm_cartesian_control");
	ros::NodeHandle node_handle;


	double rate = 25;

	//TODO: read from param
	std::string velocity_command_topic =
			"/arm_1/arm_controller/velocity_command";
	std::string joint_state_topic = "/joint_states";
	std::string cart_control_topic =
			"/arm_1/arm_controller/cartesian_velocity_command";

	std::string root_name = "arm_link_0";
	std::string tooltip_name = "arm_link_5";


	//load URDF model
	arm_cc::ROS_URDF_Loader loader;
	loader.loadModel(node_handle, root_name, tooltip_name, arm_chain, joint_limits);

	//init
	joint_positions.resize(arm_chain.getNrOfJoints());




	init_ik_solver();

	init_joint_msgs();

	//fk_solver = new KDL::ChainFkSolverPos_recursive(arm_chain);
	//jnt2jac = new KDL::ChainJntToJacSolver(arm_chain);



	//register publisher
	cmd_vel_publisher = node_handle.advertise<brics_actuator::JointVelocities>(
			velocity_command_topic, 1);

	//register subscriber
	ros::Subscriber sub_joint_states = node_handle.subscribe(joint_state_topic,
			1000, jointstateCallback);
	ros::Subscriber sub_cc = node_handle.subscribe(cart_control_topic, 1000,
			ccCallback);


	arm_cc::Arm_Cartesian_Control control(&arm_chain, ik_solver);
	std::vector<double> upper_limits;
	std::vector<double> lower_limits;

	for (unsigned int i=0; i<joint_limits.size(); i++) {
		upper_limits.push_back(joint_limits[i]->upper);
		lower_limits.push_back(joint_limits[i]->lower);
	}
	control.setJointLimits(lower_limits, upper_limits);

	KDL::JntArrayVel cmd_velocities(arm_chain.getNrOfJoints());

	//loop with 50Hz
	ros::Rate loop_rate(rate);

	while (ros::ok()) {

		ros::spinOnce();

		if(watchdog()) {
			control.process(1/rate, joint_positions, targetVelocity, cmd_velocities);

			publishJointVelocities(cmd_velocities);
		}


		loop_rate.sleep();
	}

	return 0;
}

