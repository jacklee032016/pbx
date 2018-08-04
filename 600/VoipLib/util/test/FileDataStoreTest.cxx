/*
* $Id: FileDataStoreTest.cxx,v 1.2 2007/03/02 07:29:49 lizhijie Exp $
*/
#include <iostream>
#include <cassert>

#include <FileDataStore.hxx>
#include <VIoException.hxx>
#include "cpLog.h"

int
main()
{
    cerr << "1" << endl;
    FileDataStore f(1000, "/tmp/t1");

    cerr << "2" << endl;
    f.addGroup("g1");
    assert( f.isGroup("g1") );
    assert( !f.isGroup("g1-no") );

    cerr << "3" << endl;
    f.putItem("g1", "i1", "g1i1");
    f.putItem("g1", "i2", "g1i2");
    f.putItem("g1", "i3", "g1i3");
    f.putItem("g1", "i4", "g1i4");

    cerr << "4" << endl;
    assert( f.isItem("g1", "i1") );
    assert( !f.isItem("g1", "i1-no") );

    cerr << "5" << endl;
    assert( f.getItem("g1", "i1") == "g1i1" );

    cerr << "6" << endl;
    f.removeItem("g1", "i2");
    assert( !f.isItem("g1", "i2") );

    cerr << "7" << endl;
    f.addGroup("g2");
    assert( f.isGroup("g2") );
    f.removeGroup("g2");
    assert( !f.isGroup("g2") );

    cerr << "8" << endl;
    StringList list;
    string v;
    list = f.listGroups();
    list.sort();
    assert( list.begin() != list.end() );
    v = *(list.begin());
    assert( v == "g1" );

    cerr << "9" << endl;
    list = f.listItems(string("g1"));
    list.sort();
    assert( list.begin() != list.end() );
    v = *(list.begin());
    assert( v == "i1" );

    cerr << "a" << endl;
    f.getItemSize("g1", "i1");
    cerr << "b" << endl;
    f.getItemTimeStamp("g1", "i1");
    TimeStamp t = 1;
    cerr << "c" << endl;
    f.putItem("g1", "i1", "g1i1", t);

    cerr << "d" << endl;
    f.addGroup("g3/g4");

    cerr << "e" << endl;

    try
    {
        f.putItem("g1-no", "i5", "g1i5");
        cerr << "f-no" << endl;
        assert(0);
    }
    catch (VIoException& f)
    {
        cerr << f.getDescription() << endl;
        cerr << "f-io" << endl;
    }
    catch (VException& f)
    {
        cerr << "f-exp" << endl;
    }

    cerr << "g" << endl;

    cerr << "Passed" << endl;
    return 0;
}
