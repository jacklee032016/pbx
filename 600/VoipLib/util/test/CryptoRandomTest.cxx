/*
* $Id: CryptoRandomTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "Verify.hxx"
#include "cpLog.h"
#include "CryptoRandom.hxx"
#include "Data.hxx"
#include <stdio.h>
#include "support.hxx"

void test_00()
{

    unsigned char buf[1024];

    for(int i = 0; i < 1024; ++i)
    {
	buf[i] = 0;
    }

    CryptoRandom r;
    int num = 1024; //132 bits.

    int len = r.getRandom(buf, 1024);

    int count = 0;
    for(int i = 0; i < 1024; ++i)
    {
	if(buf[i] == 0)
	{
	    ++ count;
	}
    }

    test_verify(len == 1024);
    test_verify(count < 1000);


    Data data;

    data = convertToHex(buf, num);

//    cout << "entropy value is: " << data.getData() << endl;

    //run through tests to check the strength of this random. no.
    //and then return true, based on its outcome.
//    return (len > 0 ? 1 : 0);
}


int main()
{
    test_00();
    return test_return_code(2);
}
