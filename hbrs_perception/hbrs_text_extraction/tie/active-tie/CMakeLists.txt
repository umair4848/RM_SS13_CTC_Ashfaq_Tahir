# Put all the headers in the include directory
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../atie-util/src)

# Define the main library
file(GLOB SRCS *.cpp)
add_library(active-tie STATIC
		       ActiveCamera.cpp 
 		       SharpnessMeasures.cpp
		       ActiveCameraValues.cpp 
		       Gradients.cpp
		       Integrals.cpp
		       SonyDFWVL500Values.cpp
		       OVActiveCamera.cpp	
		       OCRInterface.cpp
		       AcquisitionParametersOptimization.cpp
		       ImagePreprocessing.cpp)


