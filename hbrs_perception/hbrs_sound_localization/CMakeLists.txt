cmake_minimum_required(VERSION 2.8)
project(SoundLocalization)
string( REPLACE "\\" "/" ROSWIN32_DIR "$ENV{ROSWIN32_DIR}" )
mark_as_advanced(ROSWIN32_DIR)
set(
	CMAKE_MODULE_PATH
	"$ENV{ROSWIN32_DIR}/tools/cmake"
	)
find_package(ROSWIN32 REQUIRED)
add_definitions( "/W3 /D_CRT_SECURE_NO_WARNINGS /wd4251 /wd4275 /wd4099" ) #suppress annoying warnings
########add definition##########
ADD_DEFINITIONS(
-D WIN32
-D_DEBUG
-D_CONSOLE
-D__WINDOWS_ASIO__
-D__WINDOWS_DS__
-D_WIN32_WINNT=0x0500
-D GSL_DLL
)
################################
############include libs###########
include_directories(
.
C:/RoboCupAtHome/hbrs_common/hbrs_msgs/msg_gen/cpp/include
C:/RoboCupAtHome/hbrs_common/hbrs_srvs/srv_gen/cpp/include
"C:/external_libs/fftw/fftw-3.2.2.pl1-dll32"
"C:/external_libs/gsl library/gsl-1.8-lib/include"
"C:/Program Files/Microsoft DirectX SDK (June 2010)/Include"
)

##############################
############link directories###################
link_directories(
"C:/external_libs/fftw/fftw-3.2.2.pl1-dll32"
"C:/external_libs/gsl library/gsl-1.8-lib/lib"
"C:/Program Files/Microsoft DirectX SDK (June 2010)/Lib/x86"
)
#########################################
###########additional lib###################
link_libraries(
odbc32.lib
odbccp32.lib
dsound.lib
libgslcblas.lib
libgsl.lib
libfftw3-3.lib
libfftw3f-3.lib
libfftw3l-3.lib
${ROS_LIB}
${ROSCONSOLE_LIB}
${ROSLIB_LIB}
)
#######################################
add_executable(hbrs_sound_localization_topic
${src_folder}/ros/nodesros_t2.cpp 
./asio_driver/asio.cpp
./asio_driver/asiodrivers.cpp 
./asio_driver/iasiothiscallresolver.cpp
${src_folder}RtAudio.cpp
./asio_driver/asiolist.cpp
)

add_executable(hbrs_sound_localization_service
${src_folder}/ros/nodes/ros_t2_service.cpp
./asio_driver/asio.cpp
./asio_driver/asiodrivers.cpp 
./asio_driver/iasiothiscallresolver.cpp 
${src_folder}RtAudio.cpp
./asio_driver/asiolist.cpp
)

#target_link_libraries(
#	record 
#	${ROS_LIB}
#	${ROSCONSOLE_LIB}
#	${ROSLIB_LIB}
	#${odbc32.lib}
	#${odbccp32.lib}
	#${dsound.lib}
	#${libgslcblas.lib}
	#${libgsl.lib}
	#${libfftw3-3.lib}
	#${libfftw3f-3.lib}
	#${libfftw3l-3.lib}
#)

