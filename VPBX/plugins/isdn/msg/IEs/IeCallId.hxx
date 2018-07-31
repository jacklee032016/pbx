#ifndef __IE_CALL_ID_HXX__
#define __IE_CALL_ID_HXX__
/*
* $Id: IeCallId.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "IeContent.hxx"
#include "VException.hxx"

namespace Assist
{

/*
* p.78
* variable length IE: MT, length, content(IA5 chars)
*/
class IeCallId : public IeContent
{
	public:
		IeCallId(const unsigned char *p)  throw(IsdnIeParserException &) ;
		IeCallId( );
		IeCallId( const IeCallId& src);

		const IeCallId& operator =(const IeCallId& src);

		bool operator ==( const IeCallId& src ) const;
		IsdnIE* duplicate() const;
		virtual bool compareIsdnIE(IsdnIE* msg) const;
		
		unsigned char *encode(msg_t *msg, int ntMode);

		virtual int getIeType() const
		{
			return ISDN_IE_CALL_ID;
		};
		

#if WITH_DEBUG
		virtual const char *debugInfo();
#endif
		
	protected:

	private:
		
};
 
}

#endif

