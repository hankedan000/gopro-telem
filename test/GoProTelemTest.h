#pragma once

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class GoProTelemTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(GoProTelemTest);
	CPPUNIT_TEST(getAcclSamples);
	CPPUNIT_TEST(getGPS_Samples);
	CPPUNIT_TEST_SUITE_END();

public:
	GoProTelemTest();
	void setUp();
	void tearDown();

protected:
	void getAcclSamples();
	void getGPS_Samples();

private:

};
