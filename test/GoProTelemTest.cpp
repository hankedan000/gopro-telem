#include "GoProTelemTest.h"

#include "GoProTelem.h"

#include <sys/stat.h>
#include <unistd.h>

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
GoProTelemTest::test1()
{
}

int main()
{
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(GoProTelemTest::suite());
	return runner.run() ? 0 : EXIT_FAILURE;
}
