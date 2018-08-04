/*
* $Id: AssertSingleThreadedTest.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/

#include "AssertSingleThreaded.hxx"
#include "Lock.hxx"

int main()
{
    Vocal::Threads::SingleThreadAssertable myAssertable;

    {
	ASSERT_WRITE(t2, myAssertable);
    }
    {
	ASSERT_WRITE(t1, myAssertable);
    }

    return 0;
}

