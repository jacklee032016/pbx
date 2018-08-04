/*
 * $Id: VTest.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */

#include "global.h"
#include "VTest.hxx"
#include <iostream>

VTest::VTest(const char* testName)
        : myName(testName),
        allPassed(true)
{
    // check for and replace a trailing .cxx
    string::size_type suffix = myName.find_last_of(".cxx");
    if (suffix == myName.length() - 1)
    {
        // replace
        myName.replace(suffix - 3, 4, "");
    }
}

void VTest::test(int testNumber, bool testPassed, const char* comment = 0)
{
    if (testPassed)
    {
        cout << "PASSED: ";
    }
    else
    {
        cout << "FAILED: ";
        allPassed = false;
    }

    cout << myName << ":";
    cout << testNumber << ": ";
    if (comment)
    {
        cout << comment;
    }
    cout << endl;
}

int VTest::exitCode()
{
    if (allPassed)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
