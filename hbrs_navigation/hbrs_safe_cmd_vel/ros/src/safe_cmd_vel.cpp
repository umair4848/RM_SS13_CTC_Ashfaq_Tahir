// ROS Includes.
#include "ros/ros.h"
#include "sensor_msgs/LaserScan.h"
#include "geometry_msgs/Twist.h"

// RAW 
#include "raw_srvs/ReturnBool.h"

// Standard C++ Includes.
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>

// How close the robot is allowed to get to the wall.
#define ALLOWED_DISTANCE = 0.1

class safe_cmd_vel
{
public:

	safe_cmd_vel( ros::NodeHandle &n ) : node_handler( n )
	{
		base_velocities_subscriber = node_handler.subscribe( "/safe_cmd_vel", 1, &safe_cmd_vel::safe_cmd_vel_callback, this );
		laser_scanner_subscriber = node_handler.subscribe( "/scan_front", 1, &safe_cmd_vel::laser_scanner_callback, this );

		safe_base_velocities_publisher = node_handler.advertise<geometry_msgs::Twist>( "/cmd_vel", 1 );

		ROS_INFO( "hbrs_safe_cmd_vel has started." );
	}

	~safe_cmd_vel()
	{
		base_velocities_subscriber.shutdown(); 
		safe_base_velocities_publisher.shutdown(); 
	}

	bool start( raw_srvs::ReturnBool::Request &req, raw_srvs::ReturnBool::Response &res )
  	{
  		ROS_INFO( "hbrs_safe_cmd_vel status server has started" ); 

  		res.value = robot_to_close_to_wall; 

  		return true; 
  	}

private:

	void safe_cmd_vel_callback( const geometry_msgs::Twist& twist )
	{
		if ( twist.linear.x > 0 ) 
		{
			if( !robot_to_close_to_wall )
			{
				safe_base_velocities_publisher.publish( twist ); 
			}
			else
			{
				base_velocities.linear.x = 0.0; 
				base_velocities.linear.y = twist.linear.y; 
			}
		}

		safe_base_velocities_publisher.publish( base_velocities ); 
	}

	void laser_scanner_callback( const sensor_msgs::LaserScan &scan )
	{
		robot_to_close_to_wall = true;
	}

protected:

	ros::NodeHandle node_handler;

	ros::Subscriber base_velocities_subscriber;
	ros::Subscriber laser_scanner_subscriber;
	ros::Publisher safe_base_velocities_publisher;

	// base movement topic.
	geometry_msgs::Twist base_velocities;

	bool robot_to_close_to_wall; 

};
  
int main(int argc, char **argv)
{  
	ros::init( argc, argv, "hbrs_safe_cmd_vel" );

	ros::NodeHandle n;

	ros::spin();
	return 0;
}