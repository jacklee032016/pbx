#ifndef __IE_CAUSE_HXX__
#define __IE_CAUSE_HXX__
/*
* $Id: IeCause.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "Data.hxx"
#include "cpLog.h"

#include "IsdnIE.hxx"
#include "VException.hxx"

namespace Assist
{

#define CAUSE_MAX_LENGTH		32

#define CAUSE_LOCATION_USER		0x00		/* user */
#define CAUSE_LOCATION_LPN			0x01		/* local user private network */
#define CAUSE_LOCATION_LN			0x02		/* local user public network */
#define CAUSE_LOCATION_TN			0x03		/* transit network */
#define CAUSE_LOCATION_RN			0x04		/* remote user public network */
#define CAUSE_LOCATION_RPN		0x05		/* remote user private network */
#define CAUSE_LOCATION_INTL		0x07		/* international network */
#define CAUSE_LOCATION_BI			0x0a		/* interworking network */


#define CAUSE_T_UNALLOCATED_NUMBER				1
#define CAUSE_T_NO_ROUTE_DESTINATION				3
#define CAUSE_T_NORMAL_DISCONN					16
#define CAUSE_T_USER_BUSY							17
#define CAUSE_T_NO_USER_RESPONDING				18
#define CAUSE_T_NO_ANSWER_FROM_USER				19
#define CAUSE_T_CALL_REJECTED						21
#define CAUSE_T_NUMBER_CHANGED					22
#define CAUSE_T_DETS_IF_INACTIVE					27
#define CAUSE_T_INVALID_NUMBER_FORMAT			28		/* incomplete number */
#define CAUSE_T_FACILITY_REJECTED					29
#define CAUSE_T_UNSPECIFIED						31
#define CAUSE_T_NOCHANNEL_CIRCUIT_AVAILABLE		34
#define CAUSE_T_TMPORARY_FAILURE							41
#define CAUSE_T_REQUIRED_CHANNEL_CIRCUIT_UNAVAILABLE	44
#define CAUSE_T_RESSOURCE_UNAVAIL				47
#define CAUSE_T_SERVICE_UNAVAIL					63
#define CAUSE_T_UNIMPLEMENTED						79

/*
* page.90
*/
class IeCause : public IsdnIE
{
	public:
		IeCause(const unsigned char *p)  throw(IsdnIeParserException &) ;
		
		IeCause(int _location, int _cause);
		
		IeCause( const IeCause& src);

		const IeCause& operator =(const IeCause& src);
		bool operator ==( const IeCause& src) const;

		void setLocation(int _location )
		{
			location = _location;
		};

		int getLocation() const
		{
			return location;
		};

		void setCause( int _cause)
		{
			cause = _cause;
		};

		int getCause() const
		{
			return cause;
		};

		IsdnIE* duplicate() const;

		virtual bool compareIsdnIE(IsdnIE* msg) const;
		
		unsigned char *encode(msg_t *msg, int ntMode);

		int decode(const unsigned char *p);

		virtual int getIeType() const
		{
			return ISDN_IE_CAUSE;
		};


#if WITH_DEBUG
		virtual const char *debugInfo();
#endif
		

	protected:	
		
	private:
		int location;
		int cause;
  
};
 
}

#endif

