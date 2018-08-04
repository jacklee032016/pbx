/*
 * $Id: CircularBufferTest.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "CircularBuffer.hxx"
#include "Verify.hxx"

using Vocal::ADT::CircularBuffer;

void test(CircularBuffer<unsigned char>& buf)
{
    unsigned char buffer[256];
    buf.put((unsigned char*)"abcd", 4);
    test_verify(buf.bytesUsed() == 4);
    int actual = buf.get(buffer, 10);
    test_verify(buf.bytesUsed() == 0);
    test_verify(actual == 4);

    test_verify(strncmp((char*)buffer, "abcd", 4) == 0);
}

int main()
{
    CircularBuffer<unsigned char> buf(10);

    test(buf);
    test(buf);
    test(buf);

    return test_return_code(12);
}
