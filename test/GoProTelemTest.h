#pragma once

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class GoProTelemTest : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(GoProTelemTest);
	CPPUNIT_TEST(test1);
	CPPUNIT_TEST_SUITE_END();

public:
	GoProTelemTest();
	void setUp();
	void tearDown();

protected:
	void test1();

private:

};
