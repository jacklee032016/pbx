/*
* $Log: test_strstream.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: test_strstream.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
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
