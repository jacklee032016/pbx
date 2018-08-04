#ifndef __IE_REDIR_NR_HXX__
#define __IE_REDIR_NR_HXX__
/*
* $Id: IeRedirNR.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"

#include "IePN.hxx"

namespace Assist
{

/* IE_REDIR_NR (redirecting = during MT_SETUP) */

class IeRedirNR : public IePN
{
	public:
		IeRedirNR( const unsigned char *p)  throw(IsdnIeParserException &);
		IeRedirNR(int type, int plan, const unsigned char *number);
		IeRedirNR( const IeRedirNR& src );

		~IeRedirNR( );

		int getReason()
		{
			return reason;
		};

		IeRedirNR& operator=(const IeRedirNR& src);
		bool operator ==(const IeRedirNR& src) const;
		
		IsdnIE* duplicate() const;
		bool compareIsdnIE(IsdnIE* msg) const;
		
		void setReason(int _reason);

		unsigned char *encode(msg_t *msg, int ntMode );

		int decode(const unsigned char *p);
		
		virtual int getIeType() const
		{
			return ISDN_IE_REDIR_NR;
		};
		

#if WITH_DEBUG
		virtual const char *debugInfo();
#endif

	protected:
		int vlidateType(int _type);
		
	private:
		int reason;

		int validateReason( int _reason);
		
};

 
}

#endif

