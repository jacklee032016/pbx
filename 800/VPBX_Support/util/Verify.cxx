/*
* $Id: Verify.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include <cassert>
#include <iomanip>
#include "Verify.hxx"
#include "Lock.hxx"
#include "Mutex.hxx"

using Assist::Threads::Lock;
using Assist::Threads::Mutex;

TestStatistics* TestStatistics::myInstance = 0;
Mutex * TestStatistics::myMutex = new Mutex;

TestStatistics::TestStatistics()
    : myPassed(0),
      myFailed(0),
      myBroken(0),
      myUnexpectedPass(0)
{
}

TestStatistics::~TestStatistics()
{
}

void
TestStatistics::passed()
{
    Lock lock(*myMutex);
    if(!myInstance)
    {
	myInstance = new TestStatistics();
    }
    myInstance->myPassed++;
}


void
TestStatistics::failed()
{
    Lock lock (*myMutex);
    if(!myInstance)
    {
	myInstance = new TestStatistics();
    }
    myInstance->myFailed++;
}


void
TestStatistics::broken()
{
    Lock lock (*myMutex);
    if(!myInstance)
    {
	myInstance = new TestStatistics();
    }
    myInstance->myBroken++;
}


void
TestStatistics::unexpectedPass()
{
    Lock lock (*myMutex);
    if(!myInstance)
    {
	myInstance = new TestStatistics();
    }
    myInstance->myUnexpectedPass++;
}


void
TestStatistics::covered(const char * filename, int line, const char * label)
{
    Lock lock (*myMutex);
    if(!myInstance)
    {
	myInstance = new TestStatistics();
    }
    TestStatistics::CoverageInfo coverageInfo(filename, line, label);
    
    set<CoverageInfo>::const_iterator i 
        = myInstance->myCovered.find(coverageInfo);
    if ( i == myInstance->myCovered.end() )
    {
        myInstance->myCovered.insert(coverageInfo);
    }
}


int
TestStatistics::finish(const char* filename, int testcase_count)
{
    bool anyFailed = false;
    Lock lock (*myMutex);
    if(myInstance)
    {
	assert(filename);
	cerr << "VerifySummary:" << filename 
	     << ':' << testcase_count
	     << ':' << myInstance->myPassed
	     << ':' << myInstance->myFailed
	     << ':' << myInstance->myBroken
	     << ':' << myInstance->myUnexpectedPass
	     << "\n\n";

	cerr << "-------------------------------------------------------------"
	     << "---------\n";
	cerr << "Report for " << filename << "\n\n";

	bool brokenTest = false;

	if(
	    (myInstance->myPassed + myInstance->myFailed + 
	     myInstance->myBroken + myInstance->myUnexpectedPass)
	    != testcase_count)
	{
	    brokenTest = true;
	}

	cerr << "Summary: ";


	if(brokenTest)
	{
	    // the test is broken
	    cerr << "TEST BROKEN\n";
	}
	else if(testcase_count == myInstance->myPassed)
	{
	    // best of all possible -- everything is good
	    cerr << "PASS\n";
	}
	else if (myInstance->myFailed == 0)
	{
	    // everything's ok, but there are some broken entries
	    cerr << "PASS WITH KNOWN BUGS\n";
	}
	else if (myInstance->myFailed > 0)
	{
		cerr << "FAIL\n";
	}
	else 
	{
	    cerr << "VERIFY BUG\n";
	}
	cerr << '\n';

	cerr << "Total Number of Test Cases: " << testcase_count << '\n';
	cerr << "Number of Test Cases run: " 
	     << (myInstance->myPassed +
		 myInstance->myFailed +
		 myInstance->myBroken + 
		 myInstance->myUnexpectedPass) << "\n\n";

	cerr << "test_verify()\n";
	cerr << "    Expected:   " 
	     << myInstance->myPassed + myInstance->myFailed << '\n';
	cerr << "    Passes:     " << myInstance->myPassed << '\n';
	cerr << "    Fails:      " << myInstance->myFailed << '\n';
	cerr << '\n';
	cerr << "test_bug()\n";
	cerr << "    Expected:   " 
	     << myInstance->myBroken + myInstance->myUnexpectedPass
	     << '\n';
	cerr << "    Bugs:       " << myInstance->myBroken << '\n';
	cerr << "    Fixed Bugs: " << myInstance->myUnexpectedPass << '\n';


	cerr << "\n\n";
	if(brokenTest)
	{
	    cerr << "Help With Broken Tests:\n";

	    cerr << "The number of test cases and bugs specified in test_return_code() did not\n";
	    cerr << "match the number of times test_verify() and/or test_bug() were called.\n";
	    cerr << "To fix this, you should probably update the number of times you expect\n";
	    cerr << "test_verify() and/or test_bug() to be called in the arguments to\n";
	    cerr << "test_return_code().\n";
	}

	cerr << "============================================================"
	     << "==========\n";

        for (   set<CoverageInfo>::const_iterator i = myInstance->myCovered.begin();
                i != myInstance->myCovered.end();
                ++i
             )
        {
            cerr << i->file << ':' << i->line 
                 << ": covered label: " << i->label << endl;
        }
	delete myInstance; myInstance = 0;
    }
    if(anyFailed)
    {
	return -1;
    }
    else
    {
	return 0;
    }
}


#if 0
/** this class exists so that TestStatistics::finish() can be called
 * when the program finishes.  To do this, it calls it in the
 * destructor, and a global instance of the class is instantiated.
 */
class TestStatisticDestructorActivation
{
    public:
	TestStatisticDestructorActivation()
	{
	}

	~TestStatisticDestructorActivation()
	{
	    TestStatistics::finish(0);
	}
};

/// here is the global instance
static TestStatisticDestructorActivation junk;
#endif
