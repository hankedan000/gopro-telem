#include "GoProTelemTest.h"

#include "GoProTelem/GoProTelem.h"

#include <sys/stat.h>
#include <unistd.h>

const std::string GOPRO9_27K_60FPS_LIN_FILEPATH(TEST_VIDEOS_DIR "/gopro9_2.7K-60fps-Lin.MP4");
const std::string GOPRO9_ACCL_GRAV_PX(TEST_VIDEOS_DIR "/hero9/accl_orientation/gravity_in_+x.MP4");
const std::string GOPRO9_ACCL_GRAV_PY(TEST_VIDEOS_DIR "/hero9/accl_orientation/gravity_in_+y.MP4");
const std::string GOPRO9_ACCL_GRAV_PZ(TEST_VIDEOS_DIR "/hero9/accl_orientation/gravity_in_+z.MP4");
const std::string GOPRO9_ACCL_GRAV_NZ(TEST_VIDEOS_DIR "/hero9/accl_orientation/gravity_in_-z.MP4");

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
GoProTelemTest::getAcclSamples()
{
	gpt::MP4_Source mp4;
	CPPUNIT_ASSERT_EQUAL(0,mp4.open(GOPRO9_27K_60FPS_LIN_FILEPATH));

	const double sampRate = 200.0;// TODO get from library method
	const double samplDt = 1.0 / sampRate;

	auto samps = gpt::getAcclSamples(mp4);
	CPPUNIT_ASSERT_EQUAL(494UL, samps.size());

	double prevTimeOffset = -1;
	for (const auto &samp : samps)
	{
		if (prevTimeOffset >= 0.0)
		{
			CPPUNIT_ASSERT_DOUBLES_EQUAL(
				samplDt,
				(samp.t_offset - prevTimeOffset),
				0.0001);
		}
		prevTimeOffset = samp.t_offset;
	}
}

void
GoProTelemTest::getGPS_Samples()
{
	gpt::MP4_Source mp4;
	CPPUNIT_ASSERT_EQUAL(0,mp4.open(GOPRO9_27K_60FPS_LIN_FILEPATH));

	const double sampRate = 18.0;// TODO get from library method
	const double samplDt = 1.0 / sampRate;

	auto samps = gpt::getGPS_Samples(mp4);
	CPPUNIT_ASSERT_EQUAL(45UL, samps.size());

	double prevTimeOffset = -1;
	for (const auto &samp : samps)
	{
		if (prevTimeOffset >= 0.0)
		{
			CPPUNIT_ASSERT_DOUBLES_EQUAL(
				samplDt,
				(samp.t_offset - prevTimeOffset),
				0.002);
		}
		prevTimeOffset = samp.t_offset;
	}
}

void
GoProTelemTest::getCombinedSamples()
{
	gpt::MP4_Source mp4;
	CPPUNIT_ASSERT_EQUAL(0,mp4.open(GOPRO9_27K_60FPS_LIN_FILEPATH));

	const size_t frameCount = mp4.frameCount();
	const double sampRate = mp4.fps();
	const double samplDt = 1.0 / sampRate;

	auto samps = gpt::getCombinedTimedSamples(mp4);
	CPPUNIT_ASSERT_EQUAL(frameCount, samps.size());

	double prevTimeOffset = -1;
	for (const auto &tSamp : samps)
	{
		if (prevTimeOffset >= 0.0)
		{
			CPPUNIT_ASSERT_DOUBLES_EQUAL(
				samplDt,
				(tSamp.t_offset - prevTimeOffset),
				0.001);
		}
		prevTimeOffset = tSamp.t_offset;

		const auto &samp = tSamp.sample;
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,samp.accl.x,0.3);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(-9.8,samp.accl.y,0.3);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,samp.accl.z,0.3);
	}
}

void
GoProTelemTest::acclOrientation()
{
	// +x accl test
	{
		gpt::MP4_Source mp4;
		CPPUNIT_ASSERT_EQUAL(0,mp4.open(GOPRO9_ACCL_GRAV_PX));

		auto samps = gpt::getCombinedSamples(mp4);
		CPPUNIT_ASSERT(samps.size() > 0);

		auto samp = samps.at(0);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(9.8,samp.accl.x,1.5);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,samp.accl.y,1.5);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,samp.accl.z,1.5);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0,samp.grav.x,0.2);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,samp.grav.y,0.2);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,samp.grav.z,0.2);
	}

	// +y accl test
	{
		gpt::MP4_Source mp4;
		CPPUNIT_ASSERT_EQUAL(0,mp4.open(GOPRO9_ACCL_GRAV_PY));

		auto samps = gpt::getCombinedSamples(mp4);
		CPPUNIT_ASSERT(samps.size() > 0);

		auto samp = samps.at(0);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,samp.accl.x,1.5);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(9.8,samp.accl.y,1.5);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,samp.accl.z,1.5);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,samp.grav.x,0.2);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0,samp.grav.y,0.2);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,samp.grav.z,0.2);
	}

	// +z accl test
	{
		gpt::MP4_Source mp4;
		CPPUNIT_ASSERT_EQUAL(0,mp4.open(GOPRO9_ACCL_GRAV_PZ));

		auto samps = gpt::getCombinedSamples(mp4);
		CPPUNIT_ASSERT(samps.size() > 0);

		auto samp = samps.at(0);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,samp.accl.x,1.5);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,samp.accl.y,1.5);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(9.8,samp.accl.z,1.5);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,samp.grav.x,0.2);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,samp.grav.y,0.2);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0,samp.grav.z,0.2);
	}

	// -z accl test
	{
		gpt::MP4_Source mp4;
		CPPUNIT_ASSERT_EQUAL(0,mp4.open(GOPRO9_ACCL_GRAV_NZ));

		auto samps = gpt::getAcclSamples(mp4);
		CPPUNIT_ASSERT(samps.size() > 0);

		auto samp = samps.at(0);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0,samp.sample.x,1.5);
		CPPUNIT_ASSERT_DOUBLES_EQUAL( 0.0,samp.sample.y,1.5);
		CPPUNIT_ASSERT_DOUBLES_EQUAL(-9.8,samp.sample.z,1.5);
	}
}

int main()
{
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(GoProTelemTest::suite());
	return runner.run() ? 0 : EXIT_FAILURE;
}
