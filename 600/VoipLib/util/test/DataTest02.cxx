/*
* $Id: DataTest02.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include "mstring.hxx"
#include "cpLog.h"
#include "Data.hxx"
#include <iostream>


int main()
{
    const int MAX_TESTS = 25;

    Data data[MAX_TESTS];
    bool testPassed[MAX_TESTS];

    for (int i = 0; i < MAX_TESTS; i++)
    {
        testPassed[i] = false;
    }


    data[0] = "Testing Data";
    if (strcmp(data[0].logData(), "Testing Data") == 0)
    {
        testPassed[0] = true;
    }

    data[1] = data[0];
    if (strcmp(data[1].logData(), "Testing Data") == 0)
    {
        testPassed[1] = true;
    }

    data[2] = "Testing Data";
    data[2] += "added data";

    if (strcmp(data[2].logData(), "Testing Dataadded data") == 0)
    {
        testPassed[2] = true;
    }
    else
    {
        cerr << "test2: " << data[2].logData() << endl;
    }

    data[3] = data[1] + data[2];

    if (data[3] == "Testing DataTesting Dataadded data")
    {
        testPassed[3] = true;
    }

    string str("This is a string");
    Data data4(str);

    if (data4 == "This is a string")
    {
        testPassed[4] = true;
    }

    mstring mstr("This is a mstring");
    Data data5(mstr);

    if (data5 == "This is a mstring")
    {
        testPassed[5] = true;
    }

    int value = 5;
    Data data6(value);

    if (data6 == "5")
    {
        testPassed[6] = true;
    }

    Data testData;
    testData = "alphb";

    data[7] = "alpha";

    if ((data[7] < testData) &&
            !(data[7] == testData) &&
            !(data[7] > testData) &&
            (data[7] != testData))
    {
        testPassed[7] = true;
    }
    else
    {
        cerr << "test 7: " << endl;
        cerr << "x < testData : " << (data[7] < testData) << endl;
        cerr << "x > testData : " << (data[7] > testData) << endl;
        cerr << "x == testData : " << (data[7] == testData) << endl;
        cerr << "x != testData : " << (data[7] != testData) << endl;
    }

    data[8] = "alphc";
    if ((data[8] > testData) &&
            !(data[8] == testData) &&
            !(data[8] < testData) &&
            (data[8] != testData))
    {
        testPassed[8] = true;
    }
    else
    {
        cerr << "test 8: " << endl;
        cerr << "x < testData : " << (data[8] < testData) << endl;
        cerr << "x > testData : " << (data[8] > testData) << endl;
        cerr << "x == testData : " << (data[8] == testData) << endl;
        cerr << "x != testData : " << (data[8] != testData) << endl;
    }

    data[9] = "alphb";
    if ((data[9] == testData) &&
            !(data[9] < testData) &&
            !(data[9] > testData) &&
            !(data[9] != testData))
    {
        testPassed[9] = true;
    }
    else
    {
        cerr << "test 9: " << endl;
        cerr << "x < testData : " << (data[9] < testData) << endl;
        cerr << "x > testData : " << (data[9] > testData) << endl;
        cerr << "x == testData : " << (data[9] == testData) << endl;
        cerr << "x != testData : " << (data[9] != testData) << endl;
    }

    data[10] = "alphbx";

    if ((data[10] > testData) &&
            !(data[10] < testData) &&
            !(data[10] == testData) &&
            (data[10] != testData))
    {
        testPassed[10] = true;
    }
    else
    {
        cerr << "test 10: " << endl;
        cerr << "x < testData : " << (data[10] < testData) << endl;
        cerr << "x > testData : " << (data[10] > testData) << endl;
        cerr << "x == testData : " << (data[10] == testData) << endl;
        cerr << "x != testData : " << (data[10] != testData) << endl;
    }

    data[11] = "hello, world!";

    char newstr[256];
    strcpy(newstr, data[11].logData());

    if ((strcmp(newstr, "hello, world!") == 0) && (data[11].length() == 13))
    {
        testPassed[11] = true;
    }

    int myMax = 11;

    int allPassed = 0;

    cout << "START test program: " << __FILE__ << endl;

    for (int j = 0; j <= myMax; j++)
    {
        if (testPassed[j])
        {
            cout << "test " << j << " passed." << endl;
        }
        else
        {
            cout << "test " << j << " FAILED." << endl;
            allPassed = -1;
        }
    }

    cout << "END test program: " << __FILE__ << endl;

    return allPassed;
}

