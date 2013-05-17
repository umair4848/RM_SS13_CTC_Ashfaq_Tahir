cmake_minimum_required(VERSION 2.4.6)
include($ENV{ROS_ROOT}/core/rosbuild/rosbuild.cmake)

# Set the build type.  Options are:
#  Coverage       : w/ debug symbols, w/o optimization, w/ code-coverage
#  Debug          : w/ debug symbols, w/o optimization
#  Release        : w/o debug symbols, w/ optimization
#  RelWithDebInfo : w/ debug symbols, w/ optimization
#  MinSizeRel     : w/o debug symbols, w/ optimization, stripped binaries
#set(ROS_BUILD_TYPE RelWithDebInfo)

rosbuild_init()

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Eigen REQUIRED)
include_directories(${Eigen_INCLUDE_DIRS})

rosbuild_add_boost_directories()

find_library(LOG4CPP_LIBRARY log4cpp)

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})



INCLUDE_DIRECTORIES(
   ${PROJECT_SOURCE_DIR}/ros/include
   ${PROJECT_SOURCE_DIR}/common/include
   ${PROJECT_SOURCE_DIR}/common/johnsonAPSP/include
   ${PROJECT_SOURCE_DIR}/common/SparseAutoEncoder/include
   ${PROJECT_SOURCE_DIR}/common/include/ProbNeuralNetwork
   ${PROJECT_SOURCE_DIR}/common/src/Svm/onlineLiblinear
   ${PROJECT_SOURCE_DIR}/common/src/Svm/
   ${PROJECT_SOURCE_DIR}/common/3rdParty/emd
   ${PROJECT_SOURCE_DIR}/common/NeuralGas/
   ${PROJECT_SOURCE_DIR}/common/NeuralGas/include
)


link_libraries(
    blas
)

#uncomment if you have defined messages
#rosbuild_genmsg()
#uncomment if you have defined services
rosbuild_gensrv()

#common commands for building c++ executables and libraries
#rosbuild_add_library(${PROJECT_NAME} src/example.cpp)
#target_link_libraries(${PROJECT_NAME} another_library)
#rosbuild_add_boost_directories()
#rosbuild_link_boost(${PROJECT_NAME} thread)
#rosbuild_add_executable(example examples/example.cpp)
#target_link_libraries(example ${PROJECT_NAME})

	
######## AUTO ENCODER ###################################
rosbuild_add_library(auto_encoder
    common/src/CLogger.cpp  
    common/src/CFileSettings.cpp
    common/SparseAutoEncoder/src/CSAEToolBox.cpp 
    common/SparseAutoEncoder/src/CNeuron.cpp 
    common/SparseAutoEncoder/src/CLayer.cpp 
    common/SparseAutoEncoder/src/CNeuralNetwork.cpp 
    common/SparseAutoEncoder/src/CAutoEncoder.cpp 
    common/SparseAutoEncoder/src/CStackedAutoEncoder.cpp 
)

######### JOHNSON #####################
rosbuild_add_library(johnson_apsp
	common/johnsonAPSP/src/apsp_graph.cpp 
	common/johnsonAPSP/src/apsp_node.cpp 
	common/johnsonAPSP/src/edge.cpp
	common/johnsonAPSP/src/graph.cpp 
	common/johnsonAPSP/src/node.cpp
	common/johnsonAPSP/src/prio_queue.cpp
)

######## NEURAL GAS #####################################
rosbuild_add_library(neural_gas 
	common/NeuralGas/src/CKeyPointService_OpenSurf.cpp 
	common/NeuralGas/src/CKeyPoint.cpp 
	common/NeuralGas/3rdParty/OpenSURF/src/fasthessian.cpp
	common/NeuralGas/3rdParty/OpenSURF/src/integral.cpp 
	common/NeuralGas/3rdParty/OpenSURF/src/ipoint.cpp
	common/NeuralGas/3rdParty/OpenSURF/src/surf.cpp
	common/NeuralGas/3rdParty/OpenSURF/src/utils.cpp 
    common/NeuralGas/src/CMds.cpp 
    common/NeuralGas/src/CNeuralGasClassifier.cpp 
    common/NeuralGas/src/CNeuralGas.cpp  
    common/NeuralGas/src/CNeuralGasNode.cpp 
    common/NeuralGas/src/CNeuralGasEdge.cpp
)



#rosbuild_add_library(mds common/NeuralGas/src/CMds.cpp)
#rosbuild_add_library(pcaCv common/NeuralGas/src/CPcaCv.cpp)
#add_executable(testOpenCv common/NeuralGas/src/testOpenCv.cpp)

##########################
rosbuild_add_executable(object_categorization_geometric_node 
	ros/nodes/object_categorization_geometric_node.cpp
	common/src/CLogger.cpp)

target_link_libraries(object_categorization_geometric_node 
	CObjectCategorizationGeometric
	auto_encoder
    johnson_apsp
    neural_gas
	${LOG4CPP_LIBRARY}
)
rosbuild_link_boost(object_categorization_geometric_node signals serialization thread wserialization)

###
rosbuild_add_executable(training_object_categorization_geometric_node 
	ros/tools/training_object_categorization_geometric_node.cpp
	common/src/CLogger.cpp)

target_link_libraries(training_object_categorization_geometric_node 
	CObjectCategorizationGeometric
	auto_encoder
    johnson_apsp
    neural_gas
	${LOG4CPP_LIBRARY}
)
rosbuild_link_boost(training_object_categorization_geometric_node signals serialization thread wserialization)


###
rosbuild_add_executable(convert_models_from_text_to_binary 
	ros/tools/convert_models_from_text_to_binary.cpp
	common/src/CLogger.cpp
)
target_link_libraries(convert_models_from_text_to_binary 
	CObjectCategorizationGeometric
    auto_encoder
    johnson_apsp
    neural_gas
	${LOG4CPP_LIBRARY}
)
rosbuild_link_boost(convert_models_from_text_to_binary serialization)





###
rosbuild_add_library(CObjectCategorizationGeometric
	common/src/CObjectCategorizationGeometric.cpp 

	common/src/CObjectPseudoCategorizationGeometric.cpp)
target_link_libraries(CObjectCategorizationGeometric
	CSvmOneClassClassifier
	CSvmCascade 
	hbrs_toolbox_ros  
	CSvmClassifier
	svmLinearClassifier
	objectGeometricService
	CBSvmClassifier
)

###
rosbuild_add_library(CSvmClassifier  
	common/src/Svm/CSvmClassifier.cpp 
	common/src/Svm/svm.cpp )
	
rosbuild_add_library(CSvmOneClassClassifier  
	common/src/Svm/CSvmOneClassClassifier.cpp)
	
rosbuild_add_library(CSvmCascade  
	common/src/Svm/CSvmCascade.cpp
)

rosbuild_add_library(CBSvmClassifier  
	common/src/Svm/CBSvmClassifier.cpp)

rosbuild_add_library(svmLinearClassifier 
	common/src/Svm/onlineLiblinear/CSVMLinearClassifier.cpp
	common/src/Svm/onlineLiblinear/linear.cpp
	common/src/Svm/onlineLiblinear/tron.cpp)
	
rosbuild_add_library(objectGeometricService
	common/src/CLogger.cpp 
  	common/src/CObjectGeometricService.cpp 
	common/src/ProbNeuralNetwork/CRTPNeuralNetworkEnsemble.cpp 
	common/src/CFileSettings.cpp 
    common/src/CDensityEstimation.cpp 
	common/src/ProbNeuralNetwork/CPNeuralNetwork.cpp
	common/src/ProbNeuralNetwork/CKNNClassifier.cpp  
	common/src/ProbNeuralNetwork/CHPNeuralNetwork.cpp
	common/src/CPca.cpp)
target_link_libraries(objectGeometricService 
	hbrs_toolbox_ros 
	CRTPNeuralNetworkEnsemble 
	CObjectDecomposition)

rosbuild_add_library(CRTPNeuralNetworkEnsemble 
	common/src/CFileSettings.cpp 
	common/src/ProbNeuralNetwork/CRTPNeuralNetworkEnsemble.cpp 
	common/src/ProbNeuralNetwork/CRTPNeuralNetwork.cpp 
	common/src/CLogger.cpp 
	common/src/ProbNeuralNetwork/CPNeuralNetwork.cpp 
	common/src/ProbNeuralNetwork/CHPNeuralNetwork.cpp)

rosbuild_add_library(CObjectDecomposition 
	common/src/CObjectDecomposition.cpp 
	common/3rdParty/emd/emd.cpp)

