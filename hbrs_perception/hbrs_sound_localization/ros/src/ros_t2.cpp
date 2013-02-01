//#include "jin_record_paper.h"
//#include "speech_reco_paper.h"
///#include "RtAudio.h"
//#include "paper_sr_nr.h"
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "ros/network.h"
#include "hbrs_msgs/PointStamped.h"

#include <sstream>
#include "Localize_alg.h"
//#include "RtAudio.h"
////////////////////////
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <stdio.h>

#include <string.h>
#include <sstream>
#include<time.h>
#include <math.h>
//////////////ros include///////


////////////////////
using namespace std;

int main(int argc, char **argv)
{
	/////////ros interface////////
	ros::master::setURI("http://cob3-1-pc1:11311/");

    //set ros ip
    ros::network::setHost("cob3-1-pc3");
	ros::init(argc, argv, "hbrs_sound_localization");
	ros::NodeHandle n;
	//ros::Publisher pub = n.advertise<std_msgs::String>("/hbrs_sound_localization/localize_sound", 1000);
	ros::Publisher pub = n.advertise<hbrs_msgs::PointStamped>("/hbrs_sound_localization/localize_sound", 1000);
  	  int count = 0;
	  ros::Rate r(10);
	  Localize_thread *Localize_thread_obj; 
	  
	  double *result_vec=new double [3];
	  
	  while (n.ok())
	  {
   
     /* This is a message object. You stuff it with data, and then publish it.*/
     
		std_msgs::String msg;
		//std_msgs::String msg2;
		hbrs_msgs::PointStamped output_sound;
		//doublemsg_jin;
		std::stringstream ss;
		ss << "hello world " << count;
		//ROS_INFO("%s", ss.str().c_str());
		msg.data = ss.str();
		
		
		 Localize_thread_obj =new Localize_thread();// call localize thread
		
		Localize_thread_obj->Localize_algorithm(); 
		result_vec=Localize_thread_obj->get_final_position();
		for (int i=0;i<3;i++)
		{
			if(result_vec[i]>10 || result_vec[i]<-10)
				result_vec[i]=0;
		}
		output_sound.x=result_vec[0];
		output_sound.y=result_vec[1];
		output_sound.z=result_vec[2];
		output_sound.header.stamp=ros::Time::now();
		output_sound.header.frame_id="/microphone_array_frame";
		ROS_INFO("result microphone_array_publish,%f,%f,%f", output_sound.x,output_sound.y,output_sound.z);
		pub.publish(output_sound);
		
		
		++count;
		ros::spinOnce();
		r.sleep();
		
  }

  delete Localize_thread_obj;	

  
	//////////////begin sound localization component////////////
     //SRjin *sr_obj=new SRjin();
	
	//cout<<sin(90*PI/180)<<endl;
	return 0;//EXIT_SUCCESS;
}
