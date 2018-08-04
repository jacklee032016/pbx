#ifndef EMBEDDEDOBJ_HXX
#define EMBEDDEDOBJ_HXX

/*
* $Log: EmbeddedObj.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: EmbeddedObj.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <string>
#include <vector>
#include <map>

#include "SipRoute.hxx"
#include "global.h"
#include "SipParserMode.hxx"
#include "Data.hxx"
#include "Sptr.hxx"
#include "SipHeader.hxx"
#include "SipHeaderList.hxx"
#include "SipRawHeaderContainer.hxx"
#include "SipContact.hxx"


namespace Assist
{
    class EmbeddedObj 
    {
	    typedef map<string, char*> EscMap;
	public:

	    ///

	    EmbeddedObj(const Data& raw);

	    ///

	    EmbeddedObj(const string& raw);

	    ///

	    EmbeddedObj(const EmbeddedObj&);

	    ///

	    ~EmbeddedObj();

	    ///

	    const EmbeddedObj& operator =(const EmbeddedObj& EmbeddedObj);

	    ///

	    void decode(const Data& data);

	    ///
#if 0
	    const SipHeader& getHeader(SipHeaderType type, int index=0) const;
#endif
	    ///

	    Data getHeaderData() const { return _rawData;};

	    ///

	    Data getforwardEscapeheader() const {return _escFData;}
	    const SipRawHeaderContainer& getHeaderList() const { return _headerList; }

	    ///

	    const SipRoute& getRoute(int i = -1) const;

	    ///

	    const vector < Sptr<SipRoute> > getRouteList() const ;

            ///

	    const SipContact& getContact() const;  

	    ///

	    Data doForwardEscape(const string& src);

	    ///

	    Data doReverseEscape(const string& src);

	    ///

	    void setHeader(const Data& rawheader);     

	    ///

	    class reverseMap : public map<string, const char*> 
	    {
		public:
		    reverseMap();
	    } ;
  
	    ///
	     class forwardMap : public map<string, const char*> 
	    {
		public:
		    forwardMap();
	    };
	    
	private:
            ///
	    SipRawHeaderContainer _headerList;
	    ///
	    Data _rawData;
	    ///
	    Data _escFData;
	    static const reverseMap _escRMap;
	    static const forwardMap _escFMap;
	    bool fastDecode(Data mydata);
	    
    };
}

#endif
