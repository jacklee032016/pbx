/*
* $Id: ParsePair.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "ParsePair.hxx"
#include "vstring.hxx"
#include <cstdio>
#include <cassert>
#include <cstring>
#include <fstream>
#include <strstream>
#include <cctype>


using Assist::Configuration::ParsePair;
using Assist::Configuration::Value;
using Assist::Configuration::NameValueMap;
using Assist::ReturnCode;
using Assist::SUCCESS;
using Assist::getnext;
using Assist::getline;
using Assist::remove_quotes;
using std::string;
using std::ifstream;
using std::istrstream;


ParsePair::ParsePair()
    :   myTagName("data")
{
}


ParsePair::~ParsePair()
{
}


void
ParsePair::tagName(const string & tag_name)
{
    myTagName = tag_name;
}


ReturnCode              
ParsePair::parse(Type type, const string & s)
{
    ReturnCode rc = !SUCCESS;

    switch ( type )
    {
        case CFG_XML_FILE:
        {
            myFileName = s;
            return ( parseXMLFile() );
        }
        case CFG_XML_STRING:
        {
            return ( parseXMLString(s) );
        }
        case CFG_STRING:
        {
            istrstream  str(s.c_str(), s.size());
            return ( parseStream(str) );
        }
        case CFG_FILE:
        default:
        {
            myFileName = s;
            
            string  xml(".xml");

            size_t  file_name_size = myFileName.size();
            size_t  xml_size = xml.size();

            if (    file_name_size > 4 
                &&  myFileName.substr(file_name_size - xml_size) == xml
                )
            {
                rc = parseXMLFile();
            }
            else
            {
                rc = parseFile();
            }
        }
    }

    return ( rc );
}


const NameValueMap &
ParsePair::pairs() const
{
    return ( myPairs );
}


ReturnCode          
ParsePair::parseFile()
{
    ifstream     file(myFileName.c_str());
    
    if ( !file.is_open() )
    {
        return ( !SUCCESS );
    }

    ReturnCode rc = parseStream(file);

    file.close();
    
    return ( rc );
}

ReturnCode
ParsePair::parseStream(istream & in)
{
    string line;
    size_t line_num = 1;
    
    bool read_line = false;

    for (   read_line = getline(in, line, line_num); 
            read_line; 
            read_line = getline(in, line, line_num)
        )
    {
        string new_name = getnext(line);
        assert( new_name.size() );
        
        if ( line.size() == 0 )
        {
            Value & val = myPairs[new_name];
            ++val.count;
            val.type = NO_VALUE;
            continue;
        }
        
        Value & val = myPairs[new_name];
        ++val.count;
        val.type = VALUE_PRESENT;
        val.value.push_back(remove_quotes(line));
    }
    
    return ( SUCCESS );
}


ReturnCode          
ParsePair::parseXMLFile()
{
#if defined(ASSIST_USE_XML)
    xmlDocPtr doc = 0;

    doc = xmlParseFile(myFileName.c_str());

    if ( !doc )
    {
        return ( !SUCCESS );
    }

    ReturnCode rc = parseXML(doc);
    
    xmlFreeDoc(doc);
    
    return ( rc );

#else

    assert(0);
    return ( !SUCCESS );

#endif // ASSIST_USE_XML
}


ReturnCode
ParsePair::parseXMLString(const string & s)
{
#if defined(ASSIST_USE_XML)
    xmlDocPtr doc = 0;
    
    doc = xmlParseMemory(const_cast<char *>(s.c_str()), s.size());
    
    if ( !doc )
    {
        return ( !SUCCESS );
    }
    
    ReturnCode rc = parseXML(doc);
    
    xmlFreeDoc(doc);
    
    return ( rc );
#else
    
    assert(0);
    return ( !SUCCESS );

#endif // ASSIST_USE_XML    
}

ReturnCode
ParsePair::parseXML(xmlDocPtr & doc)
{
#if defined(ASSIST_USE_XML)

    xmlNodePtr currentNode = xmlDocGetRootElement(doc);
    
    assert( currentNode != 0 );

    string  name(reinterpret_cast<const char *>(currentNode->name));
    
    while ( name != myTagName )
    {
        currentNode = currentNode->next;

        if ( currentNode == 0 )
        {
            return ( !SUCCESS );
        }

        name = reinterpret_cast<const char *>(currentNode->name);
    }
    assert( currentNode != 0 );

    // step into the configuration data
    //

    for (   currentNode = currentNode->children;
            currentNode != 0; 
            currentNode = currentNode->next 
        )
    {
        if ( currentNode->type != XML_ELEMENT_NODE )
        {
            continue;
        }
        
        name = reinterpret_cast<const char *>(currentNode->name);
        
        const char *  pvalue 
            = reinterpret_cast<const char *>(xmlNodeGetContent(currentNode));

        string value;
        
        if ( pvalue == 0 )
        {
            // We have a name only.
            //
            Value & val = myPairs[name];
            ++val.count;
            val.type = NO_VALUE;
            
            continue;
        }
        
        value = pvalue;
        
        remove_ws(value);

        if ( value.size() == 0 )
        {
            // We have a name only.
            //
            Value & val = myPairs[name];
            ++val.count;
            val.type = NO_VALUE;
            
            continue;
        }
                
        Value & val = myPairs[name];
        ++val.count;
        val.type = VALUE_PRESENT;
        val.value.push_back(value);
        
        currentNode = currentNode->next;
    }
    
    return ( SUCCESS );
    
#else
    
    assert(0);
    return ( !SUCCESS );

#endif
    
}





