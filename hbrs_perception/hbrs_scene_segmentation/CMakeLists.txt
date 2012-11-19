cmake_minimum_required(VERSION 2.4.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
set(ROS_BUILD_TYPE Debug)

rosbuild_init()

#...: compiler options :........................................................

#...: gnu++0x
if(CMAKE_COMPILER_IS_GNUCXX)
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if(GCC_VERSION VERSION_GREATER 4.6 OR GCC_VERSION VERSION_EQUAL 4.6)
        add_definitions(-std=gnu++0x)
    else(GCC_VERSION VERSION_GREATER 4.6 OR GCC_VERSION VERSION_EQUAL 4.6)
        message(SEND_ERROR "You need GCC version 4.6 or greater to compile this package.")
    endif(GCC_VERSION VERSION_GREATER 4.6 OR GCC_VERSION VERSION_EQUAL 4.6)
endif(CMAKE_COMPILER_IS_GNUCXX)

#...: enable all but certain warnings
add_definitions(-Wall)
add_definitions(-Wno-unknown-pragmas)
add_definitions(-Wno-deprecated)
add_definitions(-fpermissive)

#...: default paths  :..........................................................

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
include_directories(
    ${PROJECT_SOURCE_DIR}/common/include
    ${PROJECT_SOURCE_DIR}/ros/include/impl
)


#...: target libraries :........................................................

rosbuild_add_library(scene_segmentation
  common/src/plane_extraction.cpp
  common/src/cloud_accumulation.cpp
  common/src/polyclipping.cpp
)
target_link_libraries(scene_segmentation polyclipping}
)

#...: target executables :......................................................

#...: workspace_finder
rosbuild_add_executable(workspace_finder
  ros/nodes/workspace_finder_node.cpp
)
target_link_libraries(workspace_finder
  scene_segmentation
)

#...: tabletop_cloud_accumulator
rosbuild_add_executable(tabletop_cloud_accumulator
  ros/nodes/tabletop_cloud_accumulator_node.cpp
)
target_link_libraries(tabletop_cloud_accumulator
  scene_segmentation
)

#...: tabletop_cloud_clusterer
rosbuild_add_executable(tabletop_cloud_clusterer
  ros/nodes/tabletop_cloud_clusterer_node.cpp
)
target_link_libraries(tabletop_cloud_clusterer
  scene_segmentation
)

#...: bounding_box_maker
rosbuild_add_executable(bounding_box_maker
  ros/nodes/bounding_box_maker_node.cpp
)
