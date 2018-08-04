#ifndef __IE_REDIR_DN_HXX__
#define __IE_REDIR_DN_HXX__
/*
* $Id: IeRedirDN.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "IePN.hxx"
#include "VException.hxx"

namespace Assist
{

/* Supplement Service specs */

/* IE_REDIR_DN (redirection = during MT_NOTIFY) */
class IeRedirDN : public IePN
{
	public:
		IeRedirDN( const unsigned char *p)  throw(IsdnIeParserException &);
		IeRedirDN(int type, int plan, const unsigned char *_number);
		IeRedirDN( const IeRedirDN& src);
		virtual ~IeRedirDN();

		bool operator ==( const IeRedirDN& src) const;
		IeRedirDN& operator = (const IeRedirDN& src);

		IsdnIE* duplicate() const;
		virtual bool compareIsdnIE(IsdnIE* msg) const;
		
		unsigned char *encode(msg_t *msg, int ntMode);

//		int decode(const char *p);
		
		virtual int getIeType() const
		{
			return ISDN_IE_REDIR_DN;
		};
		

#if WITH_DEBUG
		virtual const char *debugInfo();
#endif

	protected:
		int vlidateType(int _type);
		
	private:

			
};

 
}

#endif

