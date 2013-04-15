//#include "jin_record_paper.h"
//#include "speech_reco_paper.h"
///#include "RtAudio.h"
//#include "paper_sr_nr.h"
#include "ros/ros.h"
#include "std_msgs/String.h"
#include "ros/network.h"
#include "hbrs_srvs/ReturnPointStamped.h"
#include "hbrs_srvs/GetSoundDirection.h"
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
//#ifndef abc_H
//#define abc_H
class SoundLocalizationNode
{
private:
	ros::NodeHandle nh;
	Localize_thread *Localize_thread_obj;
	ros::ServiceServer localizeSourceOfSoundService;
	ros::ServiceServer service7;
	
//	bool activateSoundLocalization(	hbrs_srvs::ReturnPointStamped::Request& request, 
//									hbrs_srvs::ReturnPointStamped::Response& response );

public:
	
	SoundLocalizationNode()
	{
		localizeSourceOfSoundService = nh.advertiseService("/hbrs_sound_localization/do_sound_localization", &SoundLocalizationNode::activateSoundLocalization, this);
	  service7 = nh.advertiseService("/hbrs_sound_localization/get_direction", &SoundLocalizationNode::getSoundDirection, this);
	}

	~SoundLocalizationNode()
	{
		delete Localize_thread_obj;
	}

	bool activateSoundLocalization(	hbrs_srvs::ReturnPointStamped::Request& request,
									hbrs_srvs::ReturnPointStamped::Response& response )
	{
		
		cout<<"doSoundLocalization..." <<endl;
		
		double *result_vec=new double [3];
		 
		Localize_thread_obj =new Localize_thread();// call localize thread
		Localize_thread_obj->Localize_algorithm(); 
		result_vec=Localize_thread_obj->get_final_position();
		for (int i=0;i<3;i++)
		{
			if(result_vec[i]>10 || result_vec[i]<-10)
				result_vec[i]=0;
		}
		hbrs_msgs::PointStamped sound_point;

		sound_point.x=result_vec[0];
		sound_point.y=result_vec[1];
		sound_point.z=result_vec[2];
		sound_point.header.stamp=ros::Time::now();
		sound_point.header.frame_id="/microphone_array_frame";
		sound_point.ID = 0;
		ROS_INFO("result microphone_array_publish,%f,%f,%f", sound_point.x,sound_point.y,sound_point.z);
		
		response.point_stamp = sound_point;

		return true;
	}	


bool getSoundDirection(hbrs_srvs::GetSoundDirection::Request& request, hbrs_srvs::GetSoundDirection::Response& response)
{
cout<<"doSoundLocalization..." <<endl;
		
		double *result_vec=new double [3];
		 
		Localize_thread_obj =new Localize_thread();// call localize thread
		Localize_thread_obj->Localize_algorithm(); 
		result_vec=Localize_thread_obj->get_final_position();
		for (int i=0;i<3;i++)
		{
			if(result_vec[i]>10 || result_vec[i]<-10)
				result_vec[i]=0;
		}
		hbrs_msgs::PointStamped sound_point;

		sound_point.x=result_vec[0];
		sound_point.y=result_vec[1];
		sound_point.z=result_vec[2];
		sound_point.header.stamp=ros::Time::now();
		sound_point.header.frame_id="/microphone_array_frame";
		sound_point.ID = 0;
		ROS_INFO("result microphone_array_publish,%f,%f,%f", sound_point.x,sound_point.y,sound_point.z);
		

	if(sound_point.y > 0)
	{
		response.direction = hbrs_srvs::GetSoundDirection::Response::LEFT;
		return true;
	}
	else
	{
		response.direction = hbrs_srvs::GetSoundDirection::Response::RIGHT;
		return true;
	}

	return false;
}

};
//#endif
int main(int argc, char **argv)
{
	try
	{
		ros::master::setURI("http://cob3-1-pc1:11311/");
		ros::network::setHost("cob3-1-pc3");
		ros::init(argc, argv, "hbrs_sound_localization");
		
		SoundLocalizationNode *sl_node  ;
		sl_node=new SoundLocalizationNode();
		
		ros::spin();
		
	}catch(std::exception e)
	{
		ROS_ERROR("%s",e.what());
	}catch(...)
	{
		ROS_ERROR("unhandelt exception");
	}
	char c;
	std::cin>>c;
	return 0;
}


	/////////ros interface////////
/*	ros::master::setURI("http://cob3-1-pc1:11311/");

    //set ros ip
    ros::network::setHost("192.168.50.222");
	ros::init(argc, argv, "record");
	ros::NodeHandle n;
	//ros::Publisher pub = n.advertise<std_msgs::String>("/hbrs_sound_localization/localize_sound", 1000);
	ros::Publisher pub = n.advertise<geometry_msgs::PointStamped>("/hbrs_sound_localization/localize_sound", 1000);
  	  int count = 0;
	  ros::Rate r(10);
	  Localize_thread *Localize_thread_obj; 
	  while (n.ok())
	  {
   
     
		geometry_msgs::PointStamped output_sound;
		
		double *result_vec=new double [3];
		 
		Localize_thread_obj =new Localize_thread();// call localize thread
		Localize_thread_obj->Localize_algorithm(); 
		result_vec=Localize_thread_obj->get_final_position();
		for (int i=0;i<3;i++)
		{
			if(result_vec[i]>10 || result_vec[i]<-10)
				result_vec[i]=0;
		}
		output_sound.point.x=result_vec[0];
		output_sound.point.y=result_vec[1];
		output_sound.point.z=result_vec[2];
		output_sound.header.stamp=ros::Time::now();
		output_sound.header.frame_id="microphone_array";
		ROS_INFO("result microphone_array_publish,%f,%f,%f", output_sound.point.x,output_sound.point.y,output_sound.point.z);
		pub.publish(output_sound);
		
		
		++count;
		ros::spinOnce();
		r.sleep();
  }

delete Localize_thread_obj;	
  
	
	return 0;
}
*/
