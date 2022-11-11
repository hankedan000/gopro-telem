# gpmf-parser
add_library(
	gpmf-parser
		"${CMAKE_CURRENT_SOURCE_DIR}/gpmf-parser/GPMF_parser.c"
		"${CMAKE_CURRENT_SOURCE_DIR}/gpmf-parser/GPMF_utils.c"
)
target_include_directories(gpmf-parser PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/gpmf-parser/")
file(GLOB GPMF_PARSER_INCLUDE_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/gpmf-parser/GPMF_parser.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/gpmf-parser/GPMF_utils.h")
set_target_properties(
	gpmf-parser PROPERTIES
		PUBLIC_HEADER "${GPMF_PARSER_INCLUDE_FILES}"
)
add_library(
	gpmf-mp4reader
		"${CMAKE_CURRENT_SOURCE_DIR}/gpmf-parser/demo/GPMF_mp4reader.c"
)
target_include_directories(gpmf-mp4reader PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/gpmf-parser/demo/")
file(GLOB GPMF_MP4READER_INCLUDE_FILES
	"${CMAKE_CURRENT_SOURCE_DIR}/gpmf-parser/demo/GPMF_mp4reader.h")
set_target_properties(
	gpmf-mp4reader PROPERTIES
		PUBLIC_HEADER "${GPMF_MP4READER_INCLUDE_FILES}"
)