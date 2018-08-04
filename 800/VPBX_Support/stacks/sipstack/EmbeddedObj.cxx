/*
* $Log: EmbeddedObj.cxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: EmbeddedObj.cxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/


#include "EmbeddedObj.hxx"
#include "cpLog.h"
using namespace Assist;

EmbeddedObj::reverseMap const EmbeddedObj::_escRMap;
EmbeddedObj::forwardMap const EmbeddedObj::_escFMap;

EmbeddedObj::reverseMap::reverseMap()
{
    this->operator[]("20") = " ";
    this->operator[]("2C") = ",";
    this->operator[]("2c") = ",";
    this->operator[]("3D") = "=";
    this->operator[]("3d") = "=";
    this->operator[]("3B")= ";";
    this->operator[]("3b")= ";";
    this->operator[]("40")= "@";
    this->operator[]("3A")= ":";
    this->operator[]("3a")= ":";
    this->operator[]("3c")= "<";
    this->operator[]("3C")= "<";
    this->operator[]("3e")= ">";
    this->operator[]("3E")= ">";
}
EmbeddedObj::forwardMap::forwardMap()
{
    this->operator[](" ") = "%20";
    this->operator[](",") = "%2C";
    this->operator[]("=") = "%3D";
    this->operator[](";") = "%3B";
    this->operator[]("@") = "%40";
    this->operator[](":") = "%3A";
    this->operator[]("<") = "%3C";
    this->operator[](">") = "%3E";
}

EmbeddedObj::EmbeddedObj(const Data& raw)
    :
    _headerList(),
    _rawData(),
    _escFData()
{
    decode(raw);
}
EmbeddedObj::EmbeddedObj(const string& raw)
    :
    _headerList(),
    _rawData(),
    _escFData()
{
    Data row(raw);
    decode(row);
}
EmbeddedObj::EmbeddedObj(const EmbeddedObj& src)
    :
    _headerList(src._headerList),
    _rawData(src._rawData),
    _escFData(src._escFData)
{
}

const EmbeddedObj&
EmbeddedObj::operator=(const EmbeddedObj& src)
{
    if(&src !=this){
	_headerList = src._headerList;
	_rawData = src._rawData;
	_escFData = src._escFData;
    }
    return (*this);
}

EmbeddedObj::~EmbeddedObj()
{
}

void 
EmbeddedObj::decode(const Data& data)
{
    _rawData = data;
    if(fastDecode(data) == false){
	//throw exception
    }
}

#if 0
const SipHeader& 
EmbeddedObj::getHeader(SipHeaderType type, int index=0) const
{
    assert(0);   
}
#endif

const SipRoute& 
EmbeddedObj::getRoute(int i) const
{
    Sptr<SipRoute> E;
    _headerList.getParsedHeader(E, SIP_ROUTE_HDR, i);
    return *E;
}

const vector < Sptr<SipRoute> > 
EmbeddedObj::getRouteList() const
{
 vector < Sptr<SipRoute> > routes;
    for (int i=0; i<_headerList.getNumHeaders(SIP_ROUTE_HDR); i++)
    {
        Sptr<SipRoute> E;
        _headerList.getParsedHeader(E, SIP_ROUTE_HDR, i);
        routes.push_back(E);
    }
    return routes;
}

Data
EmbeddedObj::doForwardEscape(const string& src)
{
    const char *resChar = " :,@;>=<";
    string retStr;
    string::size_type copyStartPos = 0;
    string::size_type keyCharPos;
    forwardMap::const_iterator po;
    while ( (keyCharPos = src.find_first_of(resChar, copyStartPos)) != string::npos )
    {
        retStr += src.substr(copyStartPos, keyCharPos-copyStartPos);
        po = _escFMap.find(src.substr(keyCharPos,1));
        if(po != _escFMap.end()){
            retStr += po->second;
        }
        copyStartPos = keyCharPos + 1;
    }
    retStr += src.substr(copyStartPos);
    return retStr;
    //sprintf based    
}


Data
EmbeddedObj::doReverseEscape(const string & src)
{
    Data ret;
    const char* tmp = src.c_str();
    char buf[512];
    char newbuf[3];
    memset(buf,0,512);
    memset(newbuf,0,3);
    strcpy(buf, tmp);
    char *t1 = 0;
    const char *t2 = 0;
    t1 = strchr(buf, '%');
    for( ; t1 != NULL; t1 = strchr(buf, '%')){
        strncpy(newbuf, t1+1, 2);
	cpDebug(LOG_DEBUG_STACK, "Escaped Char found is = %s", newbuf);
        reverseMap::const_iterator pos;
        pos = _escRMap.find(newbuf);
        if(pos != _escRMap.end ())
        {
	    t2 = pos->second;
	    *t1 = *t2;
	    t1++;
	    for( int i = 0; i <= (int)strlen(t1 - 2); i++ )
	    {
		*(t1+i) = *(t1 + i + 2);
	    }
	    cpDebug(LOG_DEBUG_STACK, "replace value read from Static ASCII Map=%s", t2);
        }
    }
    cpDebug(LOG_DEBUG_STACK, "The Converted doReverseEscape is =%s", buf);
    ret = buf;
    return ret; 
}

bool
EmbeddedObj::fastDecode(Data str)
{
    string src = str.convertString();
    string::size_type keyCharPos;
    if((keyCharPos = src.find_first_of("%")) != string::npos)
    {
	str = doReverseEscape(src);
	str += "\r\n";
        _escFData = src;
	return _headerList.decode(&str);
    }
    else
    {
	_escFData = doForwardEscape(src);
        if(_escFData.length()){
	    return true;
	}
    }
    
    return false;
}
