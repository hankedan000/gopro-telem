#pragma once

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class MP4_SourceTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(MP4_SourceTest);
	CPPUNIT_TEST(basicTests);
	CPPUNIT_TEST_SUITE_END();

public:
	MP4_SourceTest();
	void setUp();
	void tearDown();

protected:
	void basicTests();

private:

};
