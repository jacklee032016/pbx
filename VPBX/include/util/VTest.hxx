#ifndef VTEST_HXX_
#define VTEST_HXX_

/*
* $Id: VTest.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include <string>

#ifdef WIN32
using std::string;
#endif


/* test case class */


class VTest
{
    public:
        VTest(const char* testName);
        void test(int testNumber, bool testPassed, const char* comment);
        int exitCode();
    private:
        string myName;
        bool allPassed;
};


#endif
