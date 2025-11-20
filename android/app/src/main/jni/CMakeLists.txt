cmake_minimum_required(VERSION 3.13)

project(appmodules)

set(ROOT ${CMAKE_SOURCE_DIR}/../../../../..)
set(AUDIO_API_DIR ${ROOT}/node_modules/react-native-audio-api)

include(${REACT_ANDROID_DIR}/cmake-utils/ReactNative-application.cmake)

target_sources(${CMAKE_PROJECT_NAME} PRIVATE
  ${ROOT}/shared/NativeOscillatorModule.cpp
  ${ROOT}/shared/MyOscillatorNode.cpp
)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC
    ${ROOT}/shared
    ${AUDIO_API_DIR}/common/cpp
)

add_library(react-native-audio-api SHARED IMPORTED)
string(TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_LOWER)
set_target_properties(react-native-audio-api PROPERTIES IMPORTED_LOCATION
        ${AUDIO_API_DIR}/android/build/intermediates/merged_native_libs/${BUILD_TYPE_LOWER}/merge${CMAKE_BUILD_TYPE}NativeLibs/out/lib/${CMAKE_ANDROID_ARCH_ABI}/libreact-native-audio-api.so
)
target_link_libraries(${CMAKE_PROJECT_NAME} react-native-audio-api android log)
