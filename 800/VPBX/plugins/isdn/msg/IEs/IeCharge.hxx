#ifndef __IE_CHARGE_HXX__
#define __IE_CHARGE_HXX__
/*
* $Id: IeCharge.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "IsdnIE.hxx"
#include "VException.hxx"

/*
* Advise of Charge in codeset 6, refer to 'Supplementary Service Specification Part 2' p.359 
*/

namespace Assist
{

#define IE_CHARGE_TOTAL_CHARGE				0x2

class IeCharge : public IsdnIE
{
	public:
		IeCharge(const unsigned char *p)  throw(IsdnIeParserException &) ;
		
		IeCharge(unsigned char *_indicator,  int _infoType = IE_CHARGE_TOTAL_CHARGE);
		
		IeCharge( const IeCharge& src);
		~IeCharge( );

		bool operator ==( const IeCharge& src) const;
		bool operator<( const IeCharge& src) const;
		const IeCharge& operator=(const IeCharge& src);

		IsdnIE* duplicate() const;

		virtual bool compareIsdnIE(IsdnIE* msg) const;

		void setIndicator( const unsigned char *_indicator);
		const unsigned char *getIndicator();

		int getLength();
		
		unsigned char *encode( msg_t *msg, int ntMode);

		int decode(const unsigned char *p);
		
		virtual int getIeType() const
		{
			return ISDN_IE_CHARGE;
		};


#if WITH_DEBUG
		virtual const char *debugInfo();
#endif
	protected :
	
	private:
		int 					infoType;
//		int chargeIndicator;
		unsigned char 		chargeIndicator[ISDN_IE_CONTENT_LENGTH];
		int					length;
};
 
} 

#endif

