#ifndef __IE_CALLED_PN_HXX__
#define __IE_CALLED_PN_HXX__
/*
* $Id: IeCalledPN.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"


#include "Data.hxx"

#include "VException.hxx"
//#include <map>
//#include "BaseUrl.hxx"
#include "Sptr.hxx"
#include "IeContent.hxx"

namespace Assist
{

#define PN_TYPE_UNKNOWN							0x00
#define PN_TYPE_INTERNATIONAL						0x01	/*not offered in Called PN */
#define PN_TYPE_NATIONAL							0x02
#define PN_TYPE_NETWORK							0x03
#define PN_TYPE_SUBSCRIBE							0x04

#define PN_PLAN_UNKNOWN							0x00
#define PN_PLAN_E164								0x01
#define PN_PLAN_PRIVTAE							0x09

#define PN_PRESENT_ALLOWED						0x00
#define PN_PRESENT_RESTRICTED						0x01
#define PN_PRESENT_UNAVAILABLE_INTERWORKING		0x02
#define PN_PRESENT_RESERVED						0x03


#define PN_SCREEN_USER_PROVIDED					0x01
#define PN_SCREEN_NETWORK_PROVIDED				0x03


/* Called Party Number */
/* specs, vol 5, clause 4.5.8, p.81 */
/*
* only type, plan and phoneNumber are defined for CalledPN
*/
class IeCalledPN : public IeContent
{
	public:
		IeCalledPN( const unsigned char *p)  throw(IsdnIeParserException &);
		IeCalledPN(int type, int plan, const unsigned char *number);
		IeCalledPN(const IeCalledPN&);
		IeCalledPN();
		~IeCalledPN( );

		IeCalledPN& operator = (const IeCalledPN&);
		bool operator ==(const IeCalledPN& srcReferTo) const;

		IsdnIE* duplicate() const;
		virtual bool compareIsdnIE(IsdnIE* msg) const;

		unsigned char *encode( msg_t *msg, int ntMode);

		virtual int decode(const unsigned char *p);
		
		virtual int getIeType() const
		{
			return ISDN_IE_CALLED_PN;
		};

		virtual const unsigned char *getPartyNumber();
		virtual void setPartyNumber(const unsigned char *_number);

		int getType();
		void setType(int _type);

		int getPlan();
		void setPlan(int _plan);
		

#if WITH_DEBUG
		virtual const char *debugInfo();
		const char *getTypeName();
		const char *getPlanName();
#endif
		
	protected:
		/* used both in Calling and Called Party Number */
		int 		type;
		int 		plan;
		
		virtual int validateType( int _type);

	private:
		
};
 
}

#endif

