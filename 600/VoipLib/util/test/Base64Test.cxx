/*
* $Id: Base64Test.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "Base64.hxx"
#include "VTest.hxx"
#include <iostream>

using Vocal::Base64;

bool test_00()
{
    Base64 base;
    unsigned char output[2048];  //output is twice as input.
    unsigned char input[1024];

    //the example is that given in the RFC - HTTP authentication
    strcpy((char*)(input), "Aladdin:open sesame");
    unsigned int inputLen = strlen((char*)input);

    unsigned int outputLen;

    base.encode((unsigned char*)output, &outputLen, (unsigned char*)input, inputLen);



    cout << "encoded value:" << output << endl;

    unsigned char decodeoutput[1024];
    unsigned int decodeoutputLen;

    base.decode(decodeoutput, &decodeoutputLen, output, outputLen);
    decodeoutput[decodeoutputLen]='\0';
    cout << "decoded value:" << decodeoutput << endl;

    if (strcmp ( (char*)decodeoutput, (char*)input) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int main()
{
    VTest myTest(__FILE__);

    myTest.test(0, test_00(), "Base64test 0");
    return myTest.exitCode();
}

