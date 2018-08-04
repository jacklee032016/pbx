/*
 * $Id: Writer.cxx,v 1.2 2007/03/01 20:04:32 lizhijie Exp $
 */


#include "global.h"
#include "Writer.hxx"

using std::ostream;


using Vocal::IO::Writer;


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
