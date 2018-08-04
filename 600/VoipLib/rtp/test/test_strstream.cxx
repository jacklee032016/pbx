/*
* $Id: test_strstream.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/
#include <stdio.h>
#include <strstream>
#include <string.h>
#include <string>

/*
class a
{
public:
     a();
};
 
a::a()
{
    strstream logFileNameRcv;
 
    logFileNameRcv << "udpRcv" << "-" << char(0);
 
    cerr << logFileNameRcv.str() << endl;
}
*/


int main()
{
    /*
        a* aa = new a();
     
    //    aa.fn();
     
    */
    string x;
    char* tmpstr;
    strstream logFileNameRcv;
    //   string y("abc");


    logFileNameRcv << "udpRcv" << "-" << char(0);
    cerr << logFileNameRcv.str();

    //    cerr << logFileNameRcv.str() << endl;
    tmpstr = logFileNameRcv.str();
    //    x = tmpstr;
    //    delete[] tmpstr;
    logFileNameRcv.freeze(false);

    //    x = y;

    //    delete aa;

    return 0;
};
