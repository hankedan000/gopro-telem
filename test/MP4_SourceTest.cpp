#include "MP4_SourceTest.h"

#include "GoProTelem/GoProTelem.h"

#include <sys/stat.h>
#include <unistd.h>

const std::string GOPRO9_27K_60FPS_LIN_FILEPATH(TEST_VIDEOS_DIR "/gopro9_2.7K-60fps-Lin.MP4");

MP4_SourceTest::MP4_SourceTest()
{
}

void
MP4_SourceTest::setUp()
{
	// run before each test case
}

void
MP4_SourceTest::tearDown()
{
	// run after each test case
}

void
MP4_SourceTest::mp4SourceTests()
{
	gpt::MP4_Source mp4;

	CPPUNIT_ASSERT_EQUAL(0,mp4.open(GOPRO9_27K_60FPS_LIN_FILEPATH));
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.484,mp4.duration(),0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(59.94,mp4.fps(),0.01);
	CPPUNIT_ASSERT_EQUAL(150UL,mp4.frameCount());
	CPPUNIT_ASSERT_EQUAL(3UL,mp4.payloadCount());
}

void
MP4_SourceTest::payloadTests()
{
	gpt::MP4_Source mp4;

	CPPUNIT_ASSERT_EQUAL(0,mp4.open(GOPRO9_27K_60FPS_LIN_FILEPATH));
	CPPUNIT_ASSERT_EQUAL(3UL,mp4.payloadCount());

	for (size_t pIdx=0; pIdx<3; pIdx++)
	{
		auto payload = mp4.getPayload(pIdx);
		switch (pIdx)
		{
			case 0:
				CPPUNIT_ASSERT_DOUBLES_EQUAL(0.000,payload->inTime(),0.001);
				CPPUNIT_ASSERT_DOUBLES_EQUAL(1.001,payload->outTime(),0.001);
				break;
			case 1:
				CPPUNIT_ASSERT_DOUBLES_EQUAL(1.001,payload->inTime(),0.001);
				CPPUNIT_ASSERT_DOUBLES_EQUAL(2.002,payload->outTime(),0.001);
				break;
			case 2:
				CPPUNIT_ASSERT_DOUBLES_EQUAL(2.002,payload->inTime(),0.001);
				CPPUNIT_ASSERT_DOUBLES_EQUAL(2.485,payload->outTime(),0.001);
				break;
		}
	}
}

void
MP4_SourceTest::streamTests()
{
	gpt::MP4_Source mp4;

	CPPUNIT_ASSERT_EQUAL(0,mp4.open(GOPRO9_27K_60FPS_LIN_FILEPATH));
	CPPUNIT_ASSERT_EQUAL(3UL,mp4.payloadCount());

	auto pl0 = mp4.getPayload(0);
	auto stream0 = pl0->getStream();
	CPPUNIT_ASSERT_EQUAL(true, stream0->validate(gpt::GPMF_Levels::GPMF_RECURSE_LEVELS));
	CPPUNIT_ASSERT_EQUAL(true, stream0->findNext(gpt::GPMF_KEY_STREAM, gpt::GPMF_Levels::GPMF_RECURSE_LEVELS));
	CPPUNIT_ASSERT_EQUAL(gpt::GPMF_KEY_STREAM, stream0->key());
	CPPUNIT_ASSERT_EQUAL(1U, stream0->structSize());

	auto pl1 = mp4.getPayload(1);
	auto stream1 = pl1->getStream();
	CPPUNIT_ASSERT_EQUAL(true, stream1->validate(gpt::GPMF_Levels::GPMF_RECURSE_LEVELS));
	CPPUNIT_ASSERT_EQUAL(true, stream1->findNext(gpt::GPMF_KEY_STREAM, gpt::GPMF_Levels::GPMF_RECURSE_LEVELS));
	CPPUNIT_ASSERT_EQUAL(gpt::GPMF_KEY_STREAM, stream1->key());
	CPPUNIT_ASSERT_EQUAL(1U, stream0->structSize());

	auto pl2 = mp4.getPayload(2);
	auto stream2 = pl2->getStream();
	CPPUNIT_ASSERT_EQUAL(true, stream2->validate(gpt::GPMF_Levels::GPMF_RECURSE_LEVELS));
	CPPUNIT_ASSERT_EQUAL(true, stream2->findNext(gpt::GPMF_KEY_STREAM, gpt::GPMF_Levels::GPMF_RECURSE_LEVELS));
	CPPUNIT_ASSERT_EQUAL(gpt::GPMF_KEY_STREAM, stream2->key());
	CPPUNIT_ASSERT_EQUAL(1U, stream0->structSize());

	// test stream statePush/Pop methods
	CPPUNIT_ASSERT_EQUAL(gpt::GPMF_KEY_STREAM, stream2->key());
	stream2->pushState();
	CPPUNIT_ASSERT_EQUAL(gpt::GPMF_KEY_STREAM, stream2->key());
	stream2->resetState();
	CPPUNIT_ASSERT_EQUAL(gpt::GPMF_KEY_DEVICE, stream2->key());
	stream2->popState();
	CPPUNIT_ASSERT_EQUAL(gpt::GPMF_KEY_STREAM, stream2->key());

	// test strmToString() and stream traversal
	CPPUNIT_ASSERT_EQUAL(
		std::string("STRM of ACCL of type s with 98 samples -- 3 elements per sample"),
		stream2->strmToString());
	CPPUNIT_ASSERT_EQUAL(true, stream2->findNext(gpt::GPMF_KEY_STREAM, gpt::GPMF_Levels::GPMF_RECURSE_LEVELS));
	CPPUNIT_ASSERT_EQUAL(
		std::string("STRM of GYRO of type s with 98 samples -- 3 elements per sample"),
		stream2->strmToString());
}

int main()
{
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(MP4_SourceTest::suite());
	return runner.run() ? 0 : EXIT_FAILURE;
}
