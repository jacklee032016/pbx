/*
 * $Id: VerifyExample02.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */

#include "Verify.hxx"

int equals(int a, int b)
{
    return a == b;
}

int main()
{
    test_verify(0 == 0);
    test_verify(1 == 1);
    test_verify(1 != 0);

    test_verify(equals(0, 0));
    test_verify(equals(1, 1));
    test_verify(!equals(0, 1));

    return test_return_code(6);
}

