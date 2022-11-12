#include "GoProTelemTest.h"

#include "GoProTelem/GoProTelem.h"

#include <sys/stat.h>
#include <unistd.h>

const std::string GOPRO9_27K_60FPS_LIN_FILEPATH(TEST_VIDEOS_DIR "/gopro9_2.7K-60fps-Lin.MP4");

GoProTelemTest::GoProTelemTest()
{
}

void
GoProTelemTest::setUp()
{
	// run before each test case
}

void
GoProTelemTest::tearDown()
{
	// run after each test case
}

void
GoProTelemTest::getGPS_Samples()
{
	gpt::MP4_Source mp4;
	CPPUNIT_ASSERT_EQUAL(0,mp4.open(GOPRO9_27K_60FPS_LIN_FILEPATH));

	auto gpsSamps = gpt::getGPS_Samples(mp4);
	CPPUNIT_ASSERT_EQUAL(45UL, gpsSamps.size());

	double prevTimeOffset = -1;
	for (const auto &samp : gpsSamps)
	{
		printf("%s\n", samp.toString().c_str());
		if (prevTimeOffset >= 0.0)
		{
			CPPUNIT_ASSERT_DOUBLES_EQUAL(
				0.055,
				(samp.t_offset - prevTimeOffset),
				0.002);
		}
		prevTimeOffset = samp.t_offset;
	}
}

int main()
{
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(GoProTelemTest::suite());
	return runner.run() ? 0 : EXIT_FAILURE;
}
