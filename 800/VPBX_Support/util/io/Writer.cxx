/*
* $Id: Writer.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/


#include "global.h"
#include "Writer.hxx"

using std::ostream;


using Assist::IO::Writer;


Writer::Writer()
{
}


Writer::~Writer()
{
}


ostream &
operator<<(ostream & out, const Writer & src)
{
    return ( src.writeTo(out) );
}
