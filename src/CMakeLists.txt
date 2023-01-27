add_library(GoProTelem
	GoProTelem.cpp

	FourCC.cpp
	GPMF_ErrorUtils.cpp
	GPMF_Payload.cpp
	GPMF_Stream.cpp
	MP4_Source.cpp
	SampleMath.cpp
	SampleTypes.cpp)

# add namespaced alias so that library target is referencable similar
# to how it would when included from exports. this enabled project
# users to do add_sub_directory() on this project as submodule
add_library(GoProTelem::GoProTelem ALIAS GoProTelem)

target_include_directories(GoProTelem
	PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(GoProTelem
	PRIVATE
		gpmf-mp4reader
		gpmf-parser)

if (${CMAKE_CXX_STANDARD} LESS 17)
	# for std::experimental::filesystem support
	target_link_libraries(GoProTelem
		PUBLIC
			stdc++fs)
endif()

set(LIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/GoProTelem")

# generate the version header
configure_file("${LIB_INCLUDE_DIR}/version.h.in" "include/GoProTelem/version.h")

# build a list of public header files to install
list(APPEND ${LIBNAME_ALL_UPPER}_PUBLIC_HEADERS
	"${LIB_INCLUDE_DIR}/FourCC.h"
	"${LIB_INCLUDE_DIR}/GoProTelem.h"
	"${LIB_INCLUDE_DIR}/GPMF_Payload.h"
	"${LIB_INCLUDE_DIR}/GPMF_Stream.h"
	"${LIB_INCLUDE_DIR}/MP4_Source.h"
	"${LIB_INCLUDE_DIR}/SampleMath.h"
	"${LIB_INCLUDE_DIR}/SampleTypes.h"
	"${LIB_INCLUDE_DIR}/Types.h"
	"${CMAKE_CURRENT_BINARY_DIR}/include/GoProTelem/version.h"
)
set_target_properties(GoProTelem PROPERTIES
	PUBLIC_HEADER "${${LIBNAME_ALL_UPPER}_PUBLIC_HEADERS}"
)
install(
	TARGETS GoProTelem
	EXPORT GoProTelemTargets
	ARCHIVE
		DESTINATION "${CMAKE_INSTALL_LIBDIR}/${MULTI_ARCH_SUFFIX}/${LIBRARY_INSTALL_DIR_NAME}"
	LIBRARY
		DESTINATION "${CMAKE_INSTALL_LIBDIR}/${MULTI_ARCH_SUFFIX}/${LIBRARY_INSTALL_DIR_NAME}"
	PUBLIC_HEADER
		DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_INSTALL_DIR_NAME}"
)

# so this target can be used
# export(EXPORT GoProTelemTargets
# 	FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/GoProTelemTargets.cmake"
# 	NAMESPACE GoProTelem::
# )