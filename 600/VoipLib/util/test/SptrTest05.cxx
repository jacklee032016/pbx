/*
* $Id: SptrTest05.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "Sptr.hxx"
#include "VTest.hxx"
#include <iostream>

// this test tests a simple class to make sure things get properly freed


class Test
{
    public:
        Test(int* a)
        {
            myA = a;
            (*myA)++;
        };
        ~Test()
        {
            (*myA)--;
        };
    private:
        int* myA;
};

const int start_count = 255;

bool test_00()
{
    int count = start_count;

    {
        Sptr < Test > x = new Test(&count);
        Sptr < Test > y = x;
        Sptr < Test > z = x;
        if (count != (start_count + 1))
        {
            cout << "count: " << count << endl;
            return false;  // the things weren't really allocated for
            // some reason
        }

    }

    if (count == start_count)
    {
        return true;
    }
    else
    {
        return false;
    }
}


int main()
{
    VTest myTest(__FILE__);

    myTest.test(0, test_00(), "SPtr basic functionality");

    return myTest.exitCode();
}
