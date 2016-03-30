cmake_minimum_required(VERSION 3.0)

string(REGEX REPLACE "(.*)/" "" THIS_FOLDER_NAME "${CMAKE_CURRENT_SOURCE_DIR}")
project(${THIS_FOLDER_NAME})

include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-core/script/max-pretarget.cmake)


include_directories( 
	"${C74_INCLUDES}"
)


add_library( 
	${PROJECT_NAME} 
	MODULE
	${PROJECT_NAME}.cpp
	reverb_bb.cpp
)


if (APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/../../Info.plist.in)
endif ()


include(${CMAKE_CURRENT_SOURCE_DIR}/../../max-sdk-core/script/max-posttarget.cmake)

