cmake_minimum_required (VERSION 3.1)

set(LIBNAME_ALL_UPPER "GOPROTELEM")
set(LIBNAME_ALL_LOWER "goprotelem")
# used for things like header install dir '/usr/lib/include/GoProTelem'
set(LIBRARY_INSTALL_DIR_NAME "GoProTelem")

project(GoProTelem VERSION 1.0.0)

# ---------------------------------------------------------------------------------------
# Set GOPROTELEM_MASTER_PROJECT to ON if we are building GoProTelem
# ---------------------------------------------------------------------------------------
# Check if GoProTelem is being used directly or via add_subdirectory, but allow overriding
if(NOT DEFINED GOPROTELEM_MASTER_PROJECT)
    if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
        set(GOPROTELEM_MASTER_PROJECT ON)
    else()
        set(GOPROTELEM_MASTER_PROJECT OFF)
    endif()
endif()

option(GOPROTELEM_BUILD_ALL "Build all artifacts" OFF)

# application options
option(GOPROTELEM_BUILD_APPS "Build apps" ${GOPROTELEM_MASTER_PROJECT})

# testing options
option(GOPROTELEM_BUILD_TESTS "Build tests" ${GOPROTELEM_MASTER_PROJECT})

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# build a multi-arch tuple for installing (ex. "x86_64-linux-gnu")
string(TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_LOWER)
string(TOLOWER ${CMAKE_CXX_COMPILER_ID} COMPILER_NAME_LOWER)
set(MULTI_ARCH_SUFFIX "${CMAKE_SYSTEM_PROCESSOR}-${SYSTEM_NAME_LOWER}-${COMPILER_NAME_LOWER}")

# add project's module directory to CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)

# update all submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	# Update submodules as needed
	option(GOPROTELEM_GIT_SUBMODULE "Check submodules during build" ON)
	if(GOPROTELEM_GIT_SUBMODULE)
		message(STATUS "${PROJECT_NAME} - Submodule update")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
						WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
						RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
		endif()
	endif()
endif()

add_subdirectory(dependencies)

# enable compiler warnings and treat them as errors for this library
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

# set the C++ standard
# this library depends on C++17 features like std::filesystem, but
# can falling back to C++11 via the use of std::experimental::filesystem
if(GOPROTELEM_CXX_STANDARD)
	set(CMAKE_CXX_STANDARD ${GOPROTELEM_CXX_STANDARD})
else()
	set(CMAKE_CXX_STANDARD 17)
endif()
message(STATUS "Building ${PROJECT_NAME} using C++${CMAKE_CXX_STANDARD}")

add_subdirectory(src)

# ---------------------------------------------------------------------------------------
# Build binaries
# ---------------------------------------------------------------------------------------
if (GOPROTELEM_BUILD_APPS OR GOPROTELEM_BUILD_ALL)
	message(STATUS "${PROJECT_NAME} - Generating apps")
	add_subdirectory(apps)
endif()

if (GOPROTELEM_BUILD_TESTS OR GOPROTELEM_BUILD_ALL)
	message(STATUS "${PROJECT_NAME} - Generating tests")
	include(CTest)
	add_subdirectory(test)
endif()