cmake_minimum_required (VERSION 3.1)

set(LIBNAME_ALL_UPPER "GOPRO_TELEM")
# used for things like header install dir '/usr/lib/include/gopro_telem'
set(LIBNAME_ALL_LOWER "gopro_telem")

project(GoProTelem VERSION 1.0.0)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add modules to the CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

# build a multi-arch tuple for installing (ex. "x86_64-linux-gnu")
string(TOLOWER ${CMAKE_SYSTEM_NAME} SYSTEM_NAME_LOWER)
string(TOLOWER ${CMAKE_CXX_COMPILER_ID} COMPILER_NAME_LOWER)
set(MULTI_ARCH_SUFFIX "${CMAKE_SYSTEM_PROCESSOR}-${SYSTEM_NAME_LOWER}-${COMPILER_NAME_LOWER}")

# add project's module directory to CMAKE_MODULE_PATH
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# update all submodules
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
	# Update submodules as needed
	option(GIT_SUBMODULE "Check submodules during build" ON)
	if(GIT_SUBMODULE)
		message(STATUS "Submodule update")
		execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
						WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
						RESULT_VARIABLE GIT_SUBMOD_RESULT)
		if(NOT GIT_SUBMOD_RESULT EQUAL "0")
			message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
		endif()
	endif()
endif()

add_subdirectory(dependencies)
add_subdirectory(src)

# use cppunit for testing
find_package(cppunit)
# enable testing if root project is us
if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND CPPUNIT_FOUND)
	include(CTest)
	set(BUILD_TESTING "ON")
	add_subdirectory(test)
endif()