/*
 * $Id: SipParameterList.cxx,v 1.2 2007/03/01 20:04:31 lizhijie Exp $
 */

#include "global.h"
#include "SipParameterList.hxx"
#include "LockHelper.hxx"

#include <stdio.h>

using namespace Vocal;

string
SipParameterListParserException::getName( void ) const
{
    return "SipParameterListParserException";
}


SipParameterList::SipParameterList(char delimiter /* default value */)
    : map <Data, Data>(), myDelimiter(delimiter)
{
}


SipParameterList::SipParameterList(const Data& data, 
                                   char delimiter /* default value */) 
    throw(SipParameterListParserException &)
    : map <Data, Data>(), myDelimiter(delimiter)
{
    decode(data, myDelimiter);
}


SipParameterList::SipParameterList(const SipParameterList& src )
    :map<Data, Data>(src), myDelimiter( src.myDelimiter)
{
}


SipParameterList::~SipParameterList()
{
}


Data SipParameterList::encode() const
{
    Data encoded;
    LockHelper lockh(myMutex);
    Data delim;
    delim.setchar(0, myDelimiter);
    map<Data,Data>::const_iterator i;
    bool start = true;
    for(i = begin(); i != end(); ++i)
    {
        if(i->first.length() > 0)
        {
            if(!start) encoded += delim;
            start = false;
            encoded += i->first;
            if(i->second.length() > 0)
            {
                int pos;
                Data addQuotes("\"");
                //Check to see if the value already has quotes
                pos = i->second.find("\"");
                if(pos != Data::npos) addQuotes = "";

                encoded += "=";
		if(i->first == "realm"){
                    encoded += addQuotes;
                    encoded += i->second;
                    encoded += addQuotes;
                }else if(i->first == "nonce") {
                    encoded += addQuotes;
                    encoded += i->second;
                    encoded += addQuotes;
                }else if(i->first == "domain"){
                    encoded += addQuotes;
                    encoded += i->second;
                    encoded += addQuotes;
                }else if(i->first == "username"){
                    encoded += addQuotes;
                    encoded += i->second;
                    encoded += addQuotes;
                }else if(i->first == "uri"){
                    encoded += addQuotes;
                    encoded += i->second;
                    encoded += addQuotes;
                }else if(i->first == "response"){
                    encoded += addQuotes;
                    encoded += i->second;
                    encoded += addQuotes;
                }else if(i->first == "opaque"){
                	 encoded += addQuotes;
                    encoded += i->second;
                    encoded += addQuotes;
                }else {
                    encoded += i->second;
		}
            }
        }
    }
    return encoded;
}



void SipParameterList::decode(Data data, char delimiter, bool eatWhitespace)
{
    char matchedChar;
    Data key;
    Data value;
    bool done = false;
    char del1[56], del2[56];
    myDelimiter = delimiter;
    sprintf(del1, "=%c", myDelimiter);
    sprintf(del2, "%c", myDelimiter);

    while(!done)
    {
        key = data.matchChar(del1, &matchedChar);
        if(matchedChar == '=')
        {
            // this is a separator, so stuff before this is a thing
            value = data.matchChar(del2, &matchedChar);
            if(matchedChar != myDelimiter)
            {
                value = data;
                // done
                done = true;
            }
            string tokenstr = value.convertString();
#if 0
            //Leave the parameter value in its original form
            //strip "" off this.
            int pos;
            pos = tokenstr.find("\"");

            if (pos != (int)(string::npos))
            {
                tokenstr = tokenstr.substr(pos + 1, tokenstr.length() - 2);
            }
#endif

            key.removeSpaces();
	    key.lowercase();//comaparison is case sensitive
            Data token = tokenstr;
            if(eatWhitespace)
            {
                token.removeSpaces();
            }
            operator[](key) = token;
        }
        else if(matchedChar == delimiter)
        {
            // no value here, so just stuff nothing into the value
            assert (key != "");
            operator[](key) = "";
        }
        else
        {
            // nothing left, so done
            // done
            if(data.length() != 0)
            {
                assert (data != "");
                operator[](data) = "";
            }
            done = true;
        }
    }
}

Data 
SipParameterList::getValue(const Data& key) const
{
    Data ret;
    LockHelper lock(myMutex);

    map<Data, Data>::const_iterator itr = find(key);
    if(itr != end())
    {
        ret =  itr->second;
    }
    return ret;
}


void 
SipParameterList::setValue(const Data& key, const Data& value)
{
    LockHelper lock(myMutex);
    operator[](key) = value;
}


void 
SipParameterList::clearValue(const Data& key)
{
    LockHelper lock(myMutex);
    erase(key);
}
