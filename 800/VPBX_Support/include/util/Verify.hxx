#ifndef VERIFY_HXX_
#define VERIFY_HXX_

/*
* $Id: Verify.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "global.h"
#include <iostream>
#include <set>
#include <string>

using std::set;
using std::string;
namespace Assist {
    namespace Threads {
        class Mutex;
    }
}
using Assist::Threads::Mutex;

/* Usage:
 *
 * test_verify(test_condition);
 *
 * where test_condition is a boolean condition (like in assert).
 * Unlike assert, test_verify does not stop execution.
 * 
 * 
 *
 * Here is a sample program:
 *
 * #include "Verify.hxx"
 * int main()
 * {
 *     test_verify(0 == 0);  // this should pass
 *     test_verify(1 == 0);  // this should fail
 *     return test_return_code(2);
 * }
 *
 * the output of this program:
 *
 * VerifyTestShort.cxx:5: test case '1 == 0' failed
 * VerifySummary:VerifyTestShort.cxx:2:1:1:0:0
 *
 * ----------------------------------------------------------------------
 * Report for VerifyTestShort.cxx
 * 
 * Summary: FAIL
 * 
 * Total Number of Test Cases: 2
 * Number of Test Cases run: 2
 * 
 * test_verify()
 *     Expected:   2
 *     Passes:     1
 *     Fails:      1
 * 
 * test_bug()
 *     Expected:   0
 *     Bugs:       0
 *     Fixed Bugs: 0
 * 
 * 
 * ======================================================================
 */

/// singleton class used to count the number of test cases that pass / fail
class TestStatistics
{
    public:
	/// called when a test (see below) passes
	static void passed();
	/// called when a test (see below) fails
	static void failed();
	/// called when a test (see below) is broken
	static void broken();
	/// called when a test (see below) is broken
	static void unexpectedPass();
        /// called when a coverage point is called
        static void covered(const char * filename, 
			    int line, 
			    const char * label);

    /** called by a global object that is constructed in Verify.cxx on
        finish to output a summary of results */

	static int finish(const char* filename, int testcase_count);
    protected:
	TestStatistics();
	TestStatistics(const TestStatistics&);
	~TestStatistics();

    private:
	const TestStatistics & operator=(const TestStatistics&);
	static TestStatistics * myInstance;
	static Mutex * myMutex;
	int myPassed;
	int myFailed;
	int myBroken;
	int myUnexpectedPass;

        struct CoverageInfo
        {
            CoverageInfo(const char * pFile, int pLine, const char * pLabel)
                :   file(pFile), line(pLine), label(pLabel ? pLabel : "")
            {}
            
            string      file;
            int         line;
            string      label;
            
            bool operator<(const CoverageInfo & src) const
            {
                return ( file == src.file ? line < src.line : file < src.file );
            }
        };
        set<CoverageInfo>   myCovered;
};

/** the following is a macro which implements the test system.  
 * Usage:
 *     test_verify(condition_to_verify);
 */

#define test_verify(x) \
    do { \
         if(!(x)) \
         { \
               cerr << __FILE__ << ':' << __LINE__ << ": test case '" << \
            #x << "' failed" << endl;  \
            TestStatistics::failed(); \
         } \
         else \
		  TestStatistics::passed(); \
       } while(0)

/** TODO -- replace this with support for the rest of the system to
 * understand the concept of brokenness.
 */

#define test_bug(x) \
    do { \
         if((x)) \
         { \
               cerr << __FILE__ << ':' << __LINE__ << ": test case '" << \
            #x << "' unexpected pass (passed when fail expected)" << endl;  \
            TestStatistics::unexpectedPass(); \
         } \
         else \
		  TestStatistics::broken(); \
       } while(0)

/** the following is a macro which allow a programmer to insert coverage points
 */
 
#if defined(ASSIST_TEST_COVERAGE)
    #define test_covered(x) TestStatistics::covered(__FILE__, __LINE__, (x))
#else
    #define test_covered(x)
#endif

/** the following is a macro which implements the test system.  
 * Usage:
 *     test_return_code(testcase_count) 
 */

#define test_return_code(testcase_count) \
    TestStatistics::finish(__FILE__, (testcase_count))

#endif
