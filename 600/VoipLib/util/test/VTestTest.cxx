/*
* $Id: VTestTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
// test VTest class

#include "VTest.hxx"

bool test_00()
{
    // if this were a real test, it would do something.  as it is, it
    // just passes.

    return true;
}


bool test_01()
{
    // if this were a real test, it would do something.  as it is, it
    // just fails.

    return false;
}

bool test_02()
{
    // if this were a real test, it would do something.  as it is, it
    // just fails.

    return false;
}

int main()
{
    VTest myTest(__FILE__);

    myTest.test(0, test_00(), "VTest test 0");
    myTest.test(1, test_01(), "VTest test 1");
    myTest.test(2, test_02(), "VTest test 2");

    return myTest.exitCode();
}
