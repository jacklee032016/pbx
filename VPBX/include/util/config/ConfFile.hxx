#ifndef CONFFILE_HXX_
#define CONFFILE_HXX_
/*
* $Id: ConfFile.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include <map>
#include "Data.hxx"

namespace Assist
{

class ConfFile : public map < Data, Data >
{
    public:
        ConfFile();
        bool parse( Data configFile );

};


}

#endif
