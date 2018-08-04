#ifndef __IE_CALLING_PN_HXX__
#define __IE_CALLING_PN_HXX__

/*
* $Id: IeCallingPN.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"

#include "Data.hxx"
#include "IePN.hxx"
#include "VException.hxx"

namespace Assist
{

/*
* Vol3 ,p. 85
* 0x6c, 0x04(length),0x00(number type+number plan), 0x00(present+screen), 0x83(IA5 chars), 0x31,0x30, 0x61
*/

/* Calling Party Number */
class IeCallingPN : public IePN
{
	public:
		IeCallingPN(const unsigned char *p) throw(IsdnIeParserException &);
		IeCallingPN(int _type, int _plan,  const unsigned char *number);

		IeCallingPN( const IeCallingPN& src );

		IeCallingPN();
		
		~IeCallingPN();

		const IeCallingPN& operator =(const IeCallingPN& src);
		bool operator ==(const IeCallingPN&) const ;

		IsdnIE* duplicate() const;
		virtual bool compareIsdnIE(IsdnIE* msg) const;
		
		unsigned char *encode(msg_t *msg, int ntMode);

//		int decode(const char *p);

		virtual int getIeType() const
		{
			return ISDN_IE_CALLING_PN;
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

