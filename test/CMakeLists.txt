add_executable(GoProTelemTest GoProTelemTest.cpp)
add_test(NAME GoProTelemTest COMMAND GoProTelemTest)

target_link_libraries(
	GoProTelemTest
		GoProTelem
		${CPPUNIT_LIBRARIES})
