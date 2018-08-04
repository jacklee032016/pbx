/*
* $Id: ConfFile.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/
#include "ConfFile.hxx"
#include "ParsePair.hxx"

using Assist::ConfFile;
using Assist::Configuration::NameValueMap;
using Assist::Configuration::ParsePair;

ConfFile::ConfFile()
{
}


bool
ConfFile::parse( Data configFile )
{
    ParsePair parser;
    if(parser.parse(ParsePair::CFG_FILE, 
                    configFile.convertString()) == Assist::SUCCESS)
    {
        // copy into the map
        for ( NameValueMap::const_iterator i = parser.pairs().begin();
              i != parser.pairs().end();
              ++i)
        {
            list<string>::const_iterator tmp = i->second.value.begin();
            if(i->second.type != Assist::Configuration::NO_VALUE &&
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
