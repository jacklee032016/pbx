#ifndef CONFFILE_HXX_
#define CONFFILE_HXX_
/*
 * $Id: ConfFile.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#include <map>
#include "Data.hxx"

namespace Vocal
{

class ConfFile : public map < Data, Data >
{
    public:
        ConfFile();
        bool parse( Data configFile );

};


}
#endif
