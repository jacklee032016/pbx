/*
* $Id: TimeDataCreation.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/

#include "PerfDuration.hxx"
#include "PerformanceDb.hxx"
#include "Data.hxx"
#include "CopyOnWriteData.hxx"

using Vocal::Statistics::PerfDuration;
using Vocal::Statistics::PerformanceDb;

const int COUNT = 100000;

void code()
{
    PerfDuration stat(Data("Data"));

    stat.start();
    for (int i = 0; i < COUNT; i++)
    {
	Data* container = new Data;
	delete container;
    }
    stat.stop();
    stat.record();

}

struct Empty
{
	int a;
};

void code2()
{
    PerfDuration stat(Data("Empty"));

    stat.start();
    for (int i = 0; i < COUNT; i++)
    {
	Empty* container = new Empty;
	delete container;
    }
    stat.stop();
    stat.record();
}




void code3()
{
    PerfDuration stat(Data("CopyOnWriteData"));

    stat.start();
    for (int i = 0; i < COUNT; i++)
    {
	CopyOnWriteData* container = new CopyOnWriteData;
	delete container;
    }
    stat.stop();
    stat.record();

}


struct Empty2
{
	int a;
	int b;
	char* c;
	char* d;
};


void code4()
{
    PerfDuration stat(Data("Empty2"));

    stat.start();
    for (int i = 0; i < COUNT; i++)
    {
	Empty2* container = new Empty2;
	delete container;
    }
    stat.stop();
    stat.record();

}


struct Empty3
{
	Empty3();
	int a;
	int b;
	char* c;
	char* d;
};


inline Empty3::Empty3()
    :a(0),
     b(0),
     c(0),
     d(0)
{
}

void code5()
{
    PerfDuration stat(Data("Empty3"));

    stat.start();
    for (int i = 0; i < COUNT; i++)
    {
	Empty3* container = new Empty3;
	delete container;
    }
    stat.stop();
    stat.record();

}


int main()
{
    code();
    code2();
    code3();
    code4();
    code5();

    PerformanceDb& db = PerformanceDb::instance();
    cout << db << endl;

    return 0;
}
