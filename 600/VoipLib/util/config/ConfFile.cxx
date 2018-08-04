/*
 * $Id: ConfFile.cxx,v 1.3 2007/03/03 18:38:55 lizhijie Exp $
 */


#include "ConfFile.hxx"
#include "ParsePair.hxx"

using Vocal::ConfFile;
using Vocal::Configuration::NameValueMap;
using Vocal::Configuration::ParsePair;

ConfFile::ConfFile()
{
}


bool
ConfFile::parse( Data configFile )
{
    ParsePair parser;
    if(parser.parse(ParsePair::CFG_FILE, 
                    configFile.convertString()) == Vocal::SUCCESS)
    {
        // copy into the map
        for ( NameValueMap::const_iterator i = parser.pairs().begin();
              i != parser.pairs().end();
              ++i)
        {
            list<string>::const_iterator tmp = i->second.value.begin();
            if(i->second.type != Vocal::Configuration::NO_VALUE &&
               tmp != i->second.value.end())
            {
                Data second = *tmp;
                (*this)[i->first] = second;
            }
        }
	return true;
    } 
    else
    {
	return false;
    }
}


