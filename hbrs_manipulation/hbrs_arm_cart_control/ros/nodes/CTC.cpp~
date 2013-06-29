#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/JointState.h"
#include "geometry_msgs/TwistStamped.h"
#include "math.h"

float current_pose;
float resolution = 0.001;	float vel_mag = 0.015;		int a = 0;
float *target_pose;		float target_pose_func [3];	float target_poses [17][3];
float dist_x;			float dist_y;			float dist_z;
float sign_x;			float sign_y;			float sign_z;
float vel_x;			float vel_y;			float vel_z;
float target_pose_x;		float target_pose_y;		float target_pose_z;
float initial_pose_x;		float initial_pose_y;		float initial_pose_z;
float dist_x_sqr;		float dist_y_sqr;		float dist_z_sqr;
float dist_x_mag;		float dist_y_mag;		float dist_z_mag;

ros::Publisher publisher;

//GetNewTarget
float *GetNewTarget()
{
	for (int c=0; c<=2; c++)
	{
	target_pose_func[c] = target_poses[a][c];
	}
	a++;
	return target_pose_func;
}

//Callback
void tfCallback(const sensor_msgs::JointState & msg)
{
	current_pose = msg.position[14];

	//Computation
	target_pose_x = target_pose[0];
	target_pose_y = target_pose[1];
	target_pose_z = target_pose[2];
	initial_pose_x = current_pose;
	initial_pose_y = 0.1;
	initial_pose_z = 0.01;

		//distance + velocity
		dist_x = target_pose_x - initial_pose_x;
		dist_y = target_pose_y - initial_pose_y;
		dist_z = target_pose_z - initial_pose_z;

		dist_x_sqr = powf(dist_x, 2.0);
		dist_y_sqr = powf(dist_y, 2.0);
		dist_z_sqr = powf(dist_z, 2.0);

		dist_x_mag = powf(dist_x_sqr, 0.5);
		dist_y_mag = powf(dist_y_sqr, 0.5);
		dist_z_mag = powf(dist_z_sqr, 0.5);

		if (dist_x != 0)
		{
		sign_x = dist_x/dist_x_mag;
		}
		else
		{
		sign_x = 0;
		}
		if (dist_y != 0)
		{
		sign_y = dist_y/dist_y_mag;
		}
		else
		{
		sign_y = 0;
		}
		if (dist_z != 0)
		{
		sign_z = dist_z/dist_z_mag;
		}
		else
		{
		sign_z = 0;
		}

		vel_x = sign_x * vel_mag;
		vel_y = sign_y * vel_mag;
		vel_z = sign_z * vel_mag;

		if (dist_x == 0 && dist_y == 0 && dist_z == 0)
		{
		target_pose = GetNewTarget();
		}

		//if ((dist_x == 0 && dist_y == 0 && dist_z == 0) && (a == 18))
		//{
		//return 0;
		//}

		//Publisher
		geometry_msgs::TwistStamped cart_vel;
		cart_vel.twist.linear.x = vel_x;
		cart_vel.twist.linear.y = vel_y;
		cart_vel.twist.linear.z = vel_z;
		cart_vel.header.stamp = ros::Time::now();
		publisher.publish(cart_vel);
		//
}

//Main
int main(int argc, char **argv)
{

//Target poses
float target_cord [51] = {0.275, 0.000, 0.100, 0.400, 0.150, 0.000, 0.150, 0.150, 0.000, 0.150, -0.150, 0.000, 0.400, -0.150, 0.000, 0.400, 0.150, 0.000, 0.400, 0.075, 0.050, 0.400, 0.000, 0.000, 0.275, -0.150, 0.000, 0.150, 0.000, 0.000, 0.275, 0.150, 0.000, 0.400, 0.000, 0.000, 0.150, 0.000, 0.000, 0.200, 0.075, 0.050, 0.275, 0.150, 0.000, 0.275, -0.150, 0.000, 0.275, 0.000, 0.100};

int k = 0;
for (int i=0; i<=16; i++)
{
	for (int j=0; j<=2; j++)
	{
	target_poses[i][j] = target_cord[k];
	k++;
	}
}

target_pose = GetNewTarget();
//
	ros::init(argc, argv, "CTC");
	ros::NodeHandle n;
	//Publisher
	publisher = n.advertise<geometry_msgs::TwistStamped>("/hbrs_manipulation/arm_cart_control/cartesian_velocity_command", 1000);
	//Subscriber
	ros::Subscriber sub = n.subscribe("/joint_states", 1000, tfCallback);
	while (ros::ok())
	{
	ros::spinOnce();
	}
}
