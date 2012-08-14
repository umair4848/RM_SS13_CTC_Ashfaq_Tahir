#include <ros/ros.h>
#include <ros/console.h>

class WorkspaceFinderNode
{

public:

  WorkspaceFinderNode()
  {
    ROS_INFO("Find workspace service started.");
  }

  virtual ~WorkspaceFinderNode() { };

private:

  ros::ServiceServer find_workspace_server_;

};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "workspace_finder_node");

  WorkspaceFinderNode wfn;

  ros::spin();

  return 0;
}

