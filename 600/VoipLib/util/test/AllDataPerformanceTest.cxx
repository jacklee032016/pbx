/*
* $Id: AllDataPerformanceTest.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/

#include "Verify.hxx"
#include "CopyOnWriteData.hxx"
//#include "NewData.hxx"
#include "StringData.hxx"
#include <iostream>
#include "VTime.hxx"


bool debug = false;

typedef int (*func)(int);

void test(int count, func c_f, func s_f)
{
    int c = 0;
    int s = 0;
    if (c_f)
    {
        c = c_f(count);
    }
    if (s_f)
    {
        s = s_f(count);
    }
    cout << "c: " << c << endl;
    cout << "s: " << s << endl;
}



template < class C >
int test_00_helper(int count)
{
    C test0 = "random junk asdf this is a test";
    C test1;
    C test2 = "asdf";

    VTime start;
    for (int i = 0; i < count; i++)
    {
        test0.match(test2.getData(), &test1, false);
    }
    VTime end;
    return end - start;
}


void test_00()
{
    test(100000,
         &test_00_helper < CopyOnWriteData > ,
         &test_00_helper < StringData > );
}


template < class C >
int test_01_helper(int count)
{
    C test0 = "random junk asdf this is a test";
    C test1;
    C test2 = "asdf";

    VTime start;
    for(int i = 0; i < count; i++)
    {
        test0.match("asdf", &test1, false);
    }
    VTime end;
    return end - start;
}


void test_01()
{
    test(100000,
         &test_01_helper < CopyOnWriteData > ,
         &test_01_helper < StringData > );
}


template < class C >
int test_02_helper(int count)
{
    C test0 = "random junk asdf this is a test";
    C test1;
    C test2 = "asdf";

    VTime start;
    for(int i = 0; i < count; i++)
    {
        test0.find(test2);
    }
    VTime end;
    return end - start;
}


void test_02()
{
    test(100000,
         &test_02_helper < CopyOnWriteData > ,
         &test_02_helper < StringData > );
}


int main(int argc, char** argv)
{
    test_00();
    test_01();

    test_02();

    return 0;
}
