add_library(GoProTelem GoProTelem.cpp)

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

# generate the version header
configure_file("include/version.h.in" "include/version.h")

# build a list of public header files to install
list(APPEND ${LIBNAME_ALL_UPPER}_PUBLIC_HEADERS
	"include/GoProTelem.h"
	"${CMAKE_CURRENT_BINARY_DIR}/include/version.h"
)
set_target_properties(GoProTelem PROPERTIES
	PUBLIC_HEADER "${${LIBNAME_ALL_UPPER}_PUBLIC_HEADERS}"
)
install(
	TARGETS GoProTelem
	EXPORT GoProTelemTargets
	ARCHIVE
		DESTINATION "lib/${MULTI_ARCH_SUFFIX}/${LIBNAME_ALL_LOWER}"
	LIBRARY
		DESTINATION "lib/${MULTI_ARCH_SUFFIX}/${LIBNAME_ALL_LOWER}"
	PUBLIC_HEADER
		DESTINATION "include/${LIBNAME_ALL_LOWER}"
)

# so this target can be used
# export(EXPORT GoProTelemTargets
# 	FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/GoProTelemTargets.cmake"
# 	NAMESPACE GoProTelem::
# )