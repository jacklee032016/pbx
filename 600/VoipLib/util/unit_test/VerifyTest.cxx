/*
 * $Id: VerifyTest.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "Verify.hxx"

int main()
{
    int* ptr;
    test_verify(0 == 0);
    test_verify(!(0 == 1));
    test_bug(0 == 1);

    ptr = 0;
    test_verify(ptr == 0);
    ptr = new int;
    test_verify(ptr != 0);
    *ptr = 7;
    test_verify(*ptr == 7);
    return test_return_code(6);
}
