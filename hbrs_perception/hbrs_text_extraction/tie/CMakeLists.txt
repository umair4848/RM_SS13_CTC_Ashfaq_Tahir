cmake_minimum_required (VERSION 2.8)
project(ACTIVE_VISION)

# Test files and data go here!
set(MODEL_BINARY_PATH ${PROJECT_BINARY_DIR}/models)

# Base path to the models
ADD_DEFINITIONS(-DMODEL_DATA_INSTALL_PATH=\"${CMAKE_CURRENT_SOURCE_DIR}/atie-old/models/\")


add_subdirectory(active-tie)
add_subdirectory(atie-old)
