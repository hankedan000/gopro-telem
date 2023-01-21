add_executable(gpmf2csv gpmf2csv.cpp)
target_link_libraries(gpmf2csv PRIVATE
    GoProTelem)

add_executable(gpmfdump gpmfdump.cpp)
target_link_libraries(gpmfdump PRIVATE
    GoProTelem)

install(
	TARGETS gpmf2csv gpmfdump
	RUNTIME
		DESTINATION "${CMAKE_INSTALL_BINDIR}"
)