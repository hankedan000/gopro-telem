#include "SampleMathTest.h"

#include "GoProTelem/SampleMath.h"

#include <sys/stat.h>
#include <unistd.h>

SampleMathTest::SampleMathTest()
{
}

void
SampleMathTest::setUp()
{
	// run before each test case
}

void
SampleMathTest::tearDown()
{
	// run after each test case
}

void
SampleMathTest::lerpTests()
{
	// test regular double lerp
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.5,gpt::lerp(1.0,2.0,0.5),0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0,gpt::lerp(2.0,4.0,0.5),0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0,gpt::lerp(2.0,2.0,0.5),0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0,gpt::lerp(-2.0,2.0,0.5),0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0,gpt::lerp(0.0,2.0,1.0),0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0,gpt::lerp(0.0,2.0,2.0),0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(-4.0,gpt::lerp(0.0,2.0,-2.0),0.001);

	// test accelerometer sample lerp
	gpt::AcclSample acclA = {1.0,2.0,3.0};
	gpt::AcclSample acclB = {2.0,4.0,6.0};
	gpt::AcclSample acclC;
	gpt::lerp(acclC,acclA,acclB,0.5);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.5,acclC.x,0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0,acclC.y,0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4.5,acclC.z,0.001);

	// test GPS sample lerp
	gpt::GPS_Sample gpsA = {1.0,2.0,3.0,4.0,5.0};
	gpt::GPS_Sample gpsB = {2.0,4.0,6.0,8.0,10.0};
	gpt::GPS_Sample gpsC;
	gpt::lerp(gpsC,gpsA,gpsB,0.5);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.5,gpsC.coord.lat,0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0,gpsC.coord.lon,0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4.5,gpsC.altitude,0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0,gpsC.speed2D,0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(7.5,gpsC.speed3D,0.001);

	// test timed sample lerp
	gpt::AcclTimedSample acclT_A;
	acclT_A.t_offset = 1234.0;
	acclT_A.sample.x = 1.0;
	acclT_A.sample.y = 2.0;
	acclT_A.sample.z = 3.0;
	gpt::AcclTimedSample acclT_B;
	acclT_B.t_offset = 1235.0;
	acclT_B.sample.x = 2.0;
	acclT_B.sample.y = 4.0;
	acclT_B.sample.z = 6.0;
	gpt::lerpTimedSample(acclC,acclT_A,acclT_B,1234.5);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.5,acclC.x,0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0,acclC.y,0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4.5,acclC.z,0.001);
	gpt::lerpTimedSample(acclC,acclT_A,acclT_B,1234.0);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0,acclC.x,0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0,acclC.y,0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0,acclC.z,0.001);
	gpt::lerpTimedSample(acclC,acclT_A,acclT_B,1235.0);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0,acclC.x,0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0,acclC.y,0.001);
	CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0,acclC.z,0.001);
}

void
SampleMathTest::findLerpIndexTests()
{
	const size_t NUM_SAMPS = 6;

	std::vector<gpt::AcclTimedSample> acclSamps;
	acclSamps.resize(NUM_SAMPS);
	for (unsigned int i=0; i<NUM_SAMPS; i++)
	{
		acclSamps.at(i).t_offset = i;
	}

	size_t index = 0;
	CPPUNIT_ASSERT_EQUAL(true,gpt::findLerpIndex(index,acclSamps,0.0));
	CPPUNIT_ASSERT_EQUAL(0UL,index);
	index = 0;
	CPPUNIT_ASSERT_EQUAL(true,gpt::findLerpIndex(index,acclSamps,0.5));
	CPPUNIT_ASSERT_EQUAL(0UL,index);
	index = 0;
	CPPUNIT_ASSERT_EQUAL(true,gpt::findLerpIndex(index,acclSamps,1.5));
	CPPUNIT_ASSERT_EQUAL(1UL,index);
	index = 0;
	CPPUNIT_ASSERT_EQUAL(true,gpt::findLerpIndex(index,acclSamps,3.1));
	CPPUNIT_ASSERT_EQUAL(3UL,index);
	index = 0;
	CPPUNIT_ASSERT_EQUAL(true,gpt::findLerpIndex(index,acclSamps,4.5));
	CPPUNIT_ASSERT_EQUAL(4UL,index);

	// make sure when it hits the end of the vector that it still returns
	// the last valid index in the sample vector
	index = 0;
	CPPUNIT_ASSERT_EQUAL(false,gpt::findLerpIndex(index,acclSamps,6.0));
	CPPUNIT_ASSERT_EQUAL(5UL,index);

	// shouldn't be able to find this time because we've started it's search
	// past the sample index
	index = 4;
	CPPUNIT_ASSERT_EQUAL(false,gpt::findLerpIndex(index,acclSamps,1.0));
	CPPUNIT_ASSERT_EQUAL(5UL,index);
}

int main()
{
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(SampleMathTest::suite());
	return runner.run() ? 0 : EXIT_FAILURE;
}
