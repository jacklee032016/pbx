#ifndef __IE_COMPLATE_HXX__
#define __IE_COMPLATE_HXX__
/*
* $Id: IeComplete.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "IsdnIE.hxx"
#include "VException.hxx"

namespace Assist
{

/* just a type for this IE, so no data is keep in this Class
* defined but not offered in INS 
*/
class IeComplete : public IsdnIE
{
	public:
		IeComplete(const char *p)  throw(IsdnIeParserException &) ;
		
		IeComplete( int _complete =0 );
		IeComplete( const IeComplete& src);
		~IeComplete();

		IeComplete& operator=(const IeComplete& rhs);
		
		bool operator ==( IeComplete& src) const;

		IsdnIE* duplicate() const;

		virtual bool compareIsdnIE(IsdnIE* msg) const;

		char *encode( msg_t *msg);

		int decode(const char *p);
		
		virtual int getIeType() const
		{
			return ISDN_IE_COMPLETE;
		};

		int getComplete();

		void setComplete(int _complete);

#if WITH_DEBUG
		virtual const char *debugInfo();
#endif

	protected:
		
	private:
		int complete;
};

 
}

#endif
