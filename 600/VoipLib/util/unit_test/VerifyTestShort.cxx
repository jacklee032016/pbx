/*
 * $Id: VerifyTestShort.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "Verify.hxx"
int main()
{
    test_verify(0 == 0);  // this should pass
    test_bug(1 == 0);  // this should fail
    return test_return_code(2);
}
