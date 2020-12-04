# Check the cmake version number of the system
cmake_minimum_required(VERSION 3.15)

# The name of the project
set(PROJECT_NAME classification)
project(${PROJECT_NAME} VERSION 0.0.1)

# Compile using C++17 standard
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_CXX_FLAGS "-std=c++17")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pthread")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
message(STATUS "C++17 support has been enabled by default.")

# Set build mode
set(CMAKE_BUILD_TYPE "Release")
# Set install path
set(CMAKE_INSTALL_PREFIX "/usr/local")

# Find all package.
find_package(GFlags REQUIRED)
find_package(TBB REQUIRED)
find_package(OpenCV REQUIRED)
find_package(InferenceEngine REQUIRED)
# Print all package version.
message(STATUS "GFlags   version: ${GFlags_VERSION}")
message(STATUS "TBB      version: ${TBB_VERSION}")
message(STATUS "OpenCV   version: ${OpenCV_VERSION}")
message(STATUS "OpenVINO version: ${InferenceEngine_VERSION}")

# Add all headers/library location to your include paths.
include_directories(/usr/local/include)  # System header file.
include_directories(/usr/local/include/opencv)  # OpenCV header file.
include_directories(/usr/local/include/openvino)  # OpenVINO header file.
link_directories(/usr/local/lib)  # System/OpenCV/OpenVINO library file.

add_executable(${PROJECT_NAME} classification.cpp)

# Link all libraries file
target_link_libraries(
        ${PROJECT_NAME}
        ##### Google Flags Library #####
        gflags
        ##### OpenCV Library #####
        opencv_core
        opencv_dnn
        opencv_highgui
        opencv_imgcodecs
        opencv_imgproc
        opencv_videoio
        ##### OpenVINO Library #####
        inference_engine
        inference_engine_HeteroPlugin
        inference_engine_HeteroPlugind
        inference_engine_MKLDNNPlugin
        inference_engine_MKLDNNPlugind
        inference_engine_c_api
        inference_engine_c_apid
        inference_engine_common
        inference_engine_format_reader
        inference_engine_gflags
        inference_engine_ir_reader
        inference_engine_ir_readerd
        inference_engine_legacy
        inference_engine_legacyd
        inference_engine_lp_transformations
        inference_engine_lp_transformationsd
        inference_engine_monitors
        inference_engine_myriadPlugin
        inference_engine_myriadPlugind
        inference_engine_onnx_reader
        inference_engine_onnx_readerd
        inference_engine_preproc
        inference_engine_preprocd
        inference_engine_transformations
        inference_engine_transformationsd
        inference_engined
)
