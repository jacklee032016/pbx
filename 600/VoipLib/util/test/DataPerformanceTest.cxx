/*
* $Id: DataPerformanceTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include <string>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sys/time.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include "Data2.hxx"

typedef Data TestData;

void data_performance(int buflen, int iterations, bool void_cow)
{
    long char_time, string_time, data_time;

    char* buf = new char[buflen + 1];

    int i;

    for (i = 0; i < buflen; i++)
    {
        buf[i] = random() % 26 + 'A';
    }
    buf[buflen] = '\0';

    // this is the char* time

    struct timeval start, end;

    char* dest;

    gettimeofday(&start, 0);
    for (i = 0; i < iterations; i++)
    {
        dest = new char[buflen + 1];
        memcpy(dest, buf, buflen + 1);
        if (void_cow)
        {
            dest[0] = 'z';
        }
        delete[] dest;
    }
    gettimeofday(&end, 0);

    char_time = (end.tv_sec - start.tv_sec) * 1000000 +
                (end.tv_usec - start.tv_usec);

    // this is the string time

    string strsource = buf;

    string* strdest;

    gettimeofday(&start, 0);
    for (i = 0; i < iterations; i++)
    {
        strdest = new string;
        *strdest = strsource;
        if (void_cow)
        {
            (*strdest)[0] = 'z';
        }
        delete strdest;
    }
    gettimeofday(&end, 0);

    string_time = (end.tv_sec - start.tv_sec) * 1000000 +
                  (end.tv_usec - start.tv_usec);


    // this is the string time

    TestData datasource = buf;

    TestData* datadest;

    gettimeofday(&start, 0);
    for (i = 0; i < iterations; i++)
    {
        datadest = new TestData;
        *datadest = datasource;
        if (void_cow)
        {
            datadest->setchar(0, 'z');
        }
        delete datadest;
    }
    gettimeofday(&end, 0);

    data_time = (end.tv_sec - start.tv_sec) * 1000000 +
                (end.tv_usec - start.tv_usec);

    ////////////////////////////////////////////////////////////
    // results

    printf(
        "Len: %7d  Iter: %7d  cow: %7s  char*: %8ld  string: %8ld  data: %8ld\n",
        buflen, iterations,
        (void_cow ? "invalid" : "ok"), char_time, string_time, data_time);

    delete[] buf;
    return ;
}


int main()
{
    data_performance(1, 1, false);

    data_performance(10, 10000, false);
    data_performance(100, 10000, false);
    data_performance(10000, 10000, false);
    data_performance(100000, 10000, false);


    data_performance(10000, 10, false);
    data_performance(10000, 100, false);
    data_performance(10000, 1000, false);
    data_performance(10000, 10000, false);
    data_performance(10000, 100000, false);

    data_performance(10, 10000, true);
    data_performance(100, 10000, true);
    data_performance(1000, 10000, true);
    data_performance(10000, 10000, true);

    return 0;
}
