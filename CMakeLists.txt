cmake_minimum_required(VERSION 3.0)

string(REGEX REPLACE "(.*)/" "" THIS_FOLDER_NAME "${CMAKE_CURRENT_SOURCE_DIR}")
project(${THIS_FOLDER_NAME})


# Fetch the correct verion of max-sdk-core
message(STATUS "Updating Git Submodules")
execute_process(
	COMMAND				git submodule update --init --recursive
	WORKING_DIRECTORY	"${PROJECT_SOURCE_DIR}"
)


# Update version numbers based on the latest git tag to the repository
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/package-info.json.in")
	include(${CMAKE_CURRENT_SOURCE_DIR}/source/max-sdk-core/script/git-rev.cmake)
	message("Building _____ ${GIT_TAG} _____")
	configure_file("${CMAKE_CURRENT_LIST_DIR}/package-info.json.in" "${CMAKE_CURRENT_LIST_DIR}/package-info.json" @ONLY)
endif ()

# Macro from http://stackoverflow.com/questions/7787823/cmake-how-to-get-the-name-of-all-subdirectories-of-a-directory
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()


# Now build every project using the macro above
SUBDIRLIST(PROJECT_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/source/projects)
foreach(project_dir ${PROJECT_DIRS})
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/source/projects/${project_dir}/CMakeLists.txt")
		message("Generating: ${project_dir}")
		add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/source/projects/${project_dir})
	endif()
endforeach()