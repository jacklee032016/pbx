/*
* $Id: DataAllocTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/

#include "Data.hxx"
#include "CWBuffer.hxx"
#include <list>

void test()
{
    list <Data*> myList;
    for(int i = 0; i < 100000; i++)
    {
	myList.push_back(new Data("random string"));
    }
    printBufferStatistics(cout);
    
    for(list<Data*>::iterator j = myList.begin(); j != myList.end(); j = myList.begin())
    {
	delete *j;
	myList.erase(j);
    }

    printBufferStatistics(cout);
}


int main()
{
    test();
    return 0;
}
