/*
* $Id: CopyOnWriteDataTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "CopyOnWriteData.hxx"
#include "Verify.hxx"


void  testFind()
{
    CopyOnWriteData test("asdf");

    test_verify(test.find("f") == 3);
}
    
    
void  testReplace()
{
    CopyOnWriteData test("asdf");
    CopyOnWriteData test1("asdf");
    CopyOnWriteData test2("asdf");
    CopyOnWriteData test3("asdf");
    CopyOnWriteData test4("asdf");
    CopyOnWriteData test5("asdf");
    CopyOnWriteData test6("asdf");
    CopyOnWriteData test7("asdf");
    CopyOnWriteData test8("asdf");
    CopyOnWriteData test9("asdf");
    
#if 0    
    test.replace(1,2,"e");
    cout << test.getData() << endl;
    test_verify(test == "aef");
    
    test1.replace(0,4,"e");
    cout << test1.getData() << endl;
    test_verify(test1 == "e");

    test2.replace(0,5,"e");
    cout << test2.getData() << endl;
    test_verify(test2 == "e");
    
    test3.replace(1,100,"e");
    cout << test3.getData() << endl;
    test_verify(test3 == "ae");
    
    test4.replace(3,100,"e");
    cout << test4.getData() << endl;
    test_verify(test4 == "asde");
    
    test5.replace(3,100,"lmnopqr");
    cout << test5.getData() << endl;
    test_verify(test5 == "asdlmnopqr");
    
    test6.replace(0,100,"lmnopqr");
    cout << test6.getData() << endl;
    test_verify(test6 == "lmnopqr");
    
    test7.replace(0,1,"lmnopqr");
    cout << test7.getData() << endl;
    test_verify(test7 == "lmnopqrsdf");
    
    test8.replace(1,1,"lmnopqr");
    cout << test8.getData() << endl;
    test_verify(test8 == "almnopqrdf");
    
    test9.replace(1,1,"q");
    cout << test9.getData() << endl;
    test_verify(test9 == "aqdf");
#endif    
#if 0  
    string str("asdf");
    str.replace(0,1,"lmnopqr");
    cout << "string:" << str.c_str() << endl;
#endif

    
}

int main()
{

    testFind();
    testReplace();
    return test_return_code(11);
}
