add_definitions(-DTEST_VIDEOS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test_videos")

add_executable(GoProTelemTest GoProTelemTest.cpp)
add_test(NAME GoProTelemTest COMMAND GoProTelemTest)
target_link_libraries(
	GoProTelemTest
		GoProTelem
		${CPPUNIT_LIBRARIES})

add_executable(MP4_SourceTest MP4_SourceTest.cpp)
add_test(NAME MP4_SourceTest COMMAND MP4_SourceTest)
target_link_libraries(
	MP4_SourceTest
		GoProTelem
		${CPPUNIT_LIBRARIES})

add_executable(SampleMathTest SampleMathTest.cpp)
add_test(NAME SampleMathTest COMMAND SampleMathTest)
target_link_libraries(
	SampleMathTest
		GoProTelem
		${CPPUNIT_LIBRARIES})
