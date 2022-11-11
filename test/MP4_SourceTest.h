#pragma once

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class MP4_SourceTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(MP4_SourceTest);
	CPPUNIT_TEST(mp4SourceTests);
	CPPUNIT_TEST(payloadTests);
	CPPUNIT_TEST(streamTests);
	CPPUNIT_TEST_SUITE_END();

public:
	MP4_SourceTest();
	void setUp();
	void tearDown();

protected:
	void mp4SourceTests();
	void payloadTests();
	void streamTests();

private:

};
