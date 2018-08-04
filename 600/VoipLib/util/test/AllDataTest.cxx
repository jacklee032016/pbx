/*
* $Id: AllDataTest.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/

#include "Verify.hxx"
#include "CopyOnWriteData.hxx"

#include "StringData.hxx"
#include <iostream>



bool debug = true;

typedef char* (*func)();

bool test(func c_f, func s_f)
{
    char* c0 = c_f();
    char* s0 = s_f();

    bool ok =
        ( strcmp(c0, s0) == 0);

    if (debug)
    {
        cout << "c0: " << c0 << endl;
        cout << "s0: " << s0 << endl;
    }

    free(c0);
    free(s0);

    return ok;
}



template < class C >
char* test_00_helper()
{
    string a = "   asdf is the testing   thing  ";
    C test0 = a;

    test0.removeSpaces();

    return strdup(test0.getData());
}

bool test_00()
{
    return test(&test_00_helper < CopyOnWriteData > ,
                &test_00_helper < StringData > );
}


template < class C >
char* test_01_helper()
{
    C test0 = "Foo: a,b,c,d\r\n";

    test0.expand(C("Foo:"), C(","), C("\nFoo: "), C("\n"));
    return strdup(test0.getData());
}


bool test_01()
{
    return test(&test_01_helper < CopyOnWriteData > ,
                &test_01_helper < StringData > );
}


template < class C >
char* test_02_helper()
{
    C test0 = "random junk asdf this is a test";
    C test1;
    int retn = test0.match("asdf", &test1, false);

    assert(retn == FOUND);

    return strdup(test1.getData());
}

bool test_02()
{
    return test(&test_02_helper < CopyOnWriteData > ,
                &test_02_helper < StringData > );
}


template < class C >
char* test_03_helper()
{
    C test0 = "random junk asdf this is a test";
    C test1;
    int retn = test0.match("asdf", &test1, true);

    assert(retn == FOUND);

    return strdup(test0.getData());
}

bool test_03()
{
    return test(&test_03_helper < CopyOnWriteData > ,
                &test_03_helper < StringData > );
}


template < class C >
char* test_04_helper()
{
    C test0 = "random junk asdf this is a test";
    C test1;
    int retn = test0.match("asdf", &test1, true, "hello");

    assert(retn == FOUND);

    return strdup(test0.getData());
}

bool test_04()
{
    return test(&test_04_helper < CopyOnWriteData > ,
                &test_04_helper < StringData > );
}


template < class C >
char* test_05_helper()
{
    C test0 = "random junk asdf this is a test";
    C test1;
    int retn = test0.match("asdf", &test1, true, "hello");

    assert(retn == FOUND);

    return strdup(test1.getData());
}

bool test_05()
{
    return test(&test_05_helper < CopyOnWriteData > ,
                &test_05_helper < StringData > );
}




template < class C >
char* test_06_helper()
{
    string a = "asdf is the testing   thing";
    C test0 = a;

    test0.removeSpaces();

    return strdup(test0.getData());
}

bool test_06()
{
    return test(&test_06_helper < CopyOnWriteData > ,
                &test_06_helper < StringData > );
}



int main(int argc, char** argv)
{
    if (argc > 1)
    {
        debug = true;
    }

    test_verify(test_00());
    test_verify(test_01());
    test_verify( test_02() );
    test_verify( test_03() );
    test_verify( test_04() );
    test_verify( test_05() );
    test_verify( test_06() );

    return test_return_code(6);
}
