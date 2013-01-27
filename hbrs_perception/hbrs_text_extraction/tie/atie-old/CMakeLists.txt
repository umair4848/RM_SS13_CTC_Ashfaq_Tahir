INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/)

INCLUDE_DIRECTORIES (${LIBSVM_INCLUDE_DIRS})

add_library(atie-old STATIC
			CCClassification.cpp			
			ComplexMoments.cpp
			CCBasedTIE.cpp
		    chenNiblackBinarization.cpp
			FeatureExtractors.cpp
		    parameters.cpp
		    util.cpp)
