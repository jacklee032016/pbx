#ifndef __ISDN_IE_CHANNEL_ID_HXX__
#define __ISDN_IE_CHANNEL_ID_HXX__

/*
* $Id: IeChanId.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"

#include "Data.hxx"
#include "IsdnIE.hxx"
#include "VException.hxx"

namespace Assist
{

/* bit 0-1 in octec 3 */
#define CHAN_ID_NO_CHANNEL				0x0
#define CHAN_ID_B1_CHANNEL				0x1
#define CHAN_ID_B2_CHANNEL				0x2
#define CHAN_ID_ANY_CHANNEL				0x3

/* bit 3 in octec3 */
#define CHAN_ID_MODE_PREFER				0x0
#define CHAN_ID_MODE_EXCLUSIVE			0x1	

/* 
* Vol3 p.82, only 3 octecs in PBX
* 0x18(MT), 0x01(length), 0x89(, eg, exclusively select  B1 Channel)
* octec 3 
*/
class IeChanId : public IsdnIE
{
	public:
		IeChanId(int _exclusive, int _chanId);

		IeChanId(const unsigned char *p)  throw(IsdnIeParserException &) ;
		
		IeChanId( const IeChanId& other );

		int getChanId() const
		{
			return channel;
		}

		void setChanId(int chan)
		{
			channel = chan;
		}

		void setExclusive(int mode)
		{
			exclusive = mode;
		}

		int getExclusive() const
		{
			return exclusive;
		}

		bool operator == ( const IeChanId& other ) const;

		bool operator != (const IeChanId& other) const
		{ 
			return !(*this == other);
		}

		const IeChanId& operator = ( const IeChanId& src );

		bool operator < ( const IeChanId& other ) const;

		bool operator >( const IeChanId& other ) const;

		IsdnIE* duplicate() const;

		virtual bool compareIsdnIE(IsdnIE* ie) const;

		unsigned char *encode(msg_t *msg, int ntMode);

		int decode(const unsigned char *p);
		
		virtual int getIeType() const
		{
			return ISDN_IE_CHANNEL_ID;
		};

#if WITH_DEBUG
		virtual const char *debugInfo();
#endif

	protected :	
	
	private:
		int 		channel;
		/* if preferred or exclusive */
		int 		exclusive;

};

} 

#endif

