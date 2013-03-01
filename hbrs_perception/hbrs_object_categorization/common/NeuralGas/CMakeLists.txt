cmake_minimum_required(VERSION 2.4.6)

set (CMAKE_BUILD_TYPE "Debug")
#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(JOHNSON_APSP_PATH  ${PROJECT_SOURCE_DIR}/../johnsonAPSP)

include_directories(
 .
  /opt/ros/fuerte/include/
  /opt/ros/fuerte/include/opencv
  /opt/ros/fuerte/include/opencv2
  /opt/ros/fuerte/include/opencv2/core
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/3rdParty/src
  
  ${JOHNSON_APSP_PATH}/include
)

link_directories(
  /usr/local/lib
  /opt/ros/fuerte/stacks/vision_opencv/opencv2/opencv/lib
  /opt/ros/fuerte/lib
  #/opt/ros/diamondback/stacks/vision_opencv/opencv2/opencv/lib
  ${PROJECT_SOURCE_DIR}/3rdParty/src
  ${PROJECT_SOURCE_DIR}/lib
  ${JOHNSON_APSP_PATH}/lib
)


link_libraries(	
  opencv_highgui
  opencv_core
#  cv
#  highgui
#  cxcore
  johnsonAPSP
)

add_library(keyPoint 
${PROJECT_SOURCE_DIR}/src/CKeyPointService_OpenSurf.cpp 
${PROJECT_SOURCE_DIR}/src/CKeyPoint.cpp 
${PROJECT_SOURCE_DIR}/3rdParty/OpenSURF/src/fasthessian.cpp
${PROJECT_SOURCE_DIR}/3rdParty/OpenSURF/src/integral.cpp 
${PROJECT_SOURCE_DIR}/3rdParty/OpenSURF/src/ipoint.cpp
${PROJECT_SOURCE_DIR}/3rdParty/OpenSURF/src/surf.cpp
${PROJECT_SOURCE_DIR}/3rdParty/OpenSURF/src/utils.cpp 
)


add_library(toolBoxNG src/CToolBoxNG.cpp)

add_library(neuralGasEdge src/CNeuralGasEdge.cpp src/CNeuralGasNode.cpp)

add_library(neuralGasNode src/CNeuralGasNode.cpp src/CNeuralGasEdge.cpp)

add_library(neuralGas src/CToolBoxNG.cpp src/CNeuralGas.cpp  src/CNeuralGasNode.cpp src/CNeuralGasEdge.cpp src/CMds.cpp)

#add_library(mds src/CMds.cpp)
add_library(pcaCv SHARED src/CPcaCv.cpp)

add_library(neuralGasClassifier SHARED src/CMds.cpp src/CNeuralGasClassifier src/CNeuralGas.cpp  src/CNeuralGasNode.cpp src/CNeuralGasEdge.cpp)

