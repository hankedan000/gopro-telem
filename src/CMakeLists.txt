add_library(Math Math.cpp)

# add namespaced alias so that library target is referencable similar
# to how it would when included from exports. this enabled project
# users to do add_sub_directory() on this project as submodule
add_library(Math::Math ALIAS Math)

target_include_directories(Math
	PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/include")

# generate the version header
configure_file("include/version.h.in" "include/version.h")

# build a list of public header files to install
list(APPEND ${LIBNAME_ALL_UPPER}_PUBLIC_HEADERS
	"include/Math.h"
	"${CMAKE_CURRENT_BINARY_DIR}/include/version.h"
)
set_target_properties(Math PROPERTIES
	PUBLIC_HEADER "${${LIBNAME_ALL_UPPER}_PUBLIC_HEADERS}"
)
install(
	TARGETS Math
	EXPORT MathTargets
	ARCHIVE
		DESTINATION "lib/${MULTI_ARCH_SUFFIX}/${LIBNAME_ALL_LOWER}"
	LIBRARY
		DESTINATION "lib/${MULTI_ARCH_SUFFIX}/${LIBNAME_ALL_LOWER}"
	PUBLIC_HEADER
		DESTINATION "include/${LIBNAME_ALL_LOWER}"
)

# so this target can be used
export(EXPORT MathTargets
	FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/MathTargets.cmake"
	NAMESPACE Math::
)