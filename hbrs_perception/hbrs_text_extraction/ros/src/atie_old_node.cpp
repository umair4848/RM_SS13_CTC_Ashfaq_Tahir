#include <active-tie/ActiveCamera.hpp>
#include <active-tie/OVActiveCamera.hpp>
#include <active-tie/SonyDFWVL500Values.hpp>
#include <active_tie/ActiveCameraROS.hpp>
#include <active-tie/AcquisitionParametersOptimization.hpp>
#include <active-tie/OCRInterface.hpp>
#include <active_tie/TIEROSInterface.hpp>
#include <active-tie/ActiveCamera.hpp>
#include <active-tie/SonyDFWVL500Values.hpp>
#include <atie_old/OldTIEROSInterface.hpp> 
#include <atie-old/CCBasedTIE.hpp>

int main(int argc, char** argv) {
  ros::init(argc, argv, "atie_old_node");
  ros::NodeHandle nh;
  initParameters();
  init();
  atie::ActiveCameraValues* config= new atie::SonyDFWVL500Values();
  atie::ActiveCamera* camera= new atie::ActiveCameraROS("image", NULL, 
							config);
  camera->view(false);
  atie::OVActiveCamera cameraOptim (camera);
  cameraOptim.setOptimizeView(false);
  cameraOptim.setNFramesToAverage(0);
  TIEROSInterface* ri =
   new OldTIEROSInterface(&cameraOptim);
  ros::spin();
  return 0;
}
