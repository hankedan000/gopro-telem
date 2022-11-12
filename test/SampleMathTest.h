#pragma once

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class SampleMathTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(SampleMathTest);
	CPPUNIT_TEST(lerpTests);
	CPPUNIT_TEST(findLerpIndexTests);
	CPPUNIT_TEST_SUITE_END();

public:
	SampleMathTest();
	void setUp();
	void tearDown();

protected:
	void lerpTests();
	void findLerpIndexTests();

private:

};
