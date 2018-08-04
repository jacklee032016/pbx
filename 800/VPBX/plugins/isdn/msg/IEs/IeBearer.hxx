#ifndef __ISDN_IE_BEARER_HXX__
#define __ISDN_IE_BEARER_HXX__

/*
* $Id: IeBearer.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "global.h"
#include "IsdnIE.hxx"
#include "VException.hxx"

namespace Assist
{

#define 	BEARER_INFO_CODING_TTC									0x0

/* bearer capability, p.74 */
#define	BEARER_INFO_CAPABILITY_SPEECH							0x00
#define	BEARER_INFO_CAPABILITY_DIGITAL_UNRESTRICTED			0x23
#define	BEARER_INFO_CAPABILITY_31KHZ_AUDIO						0x10

//#define	BEARER_INFO_CAPABILITY_VIDEO								0x22
//#define	BEARER_INFO_CAPABILITY_DIGITAL_RESTRICTED				0x24
//#define	BEARER_INFO_CAPABILITY_DIGITAL_UNRESTRICTED_TONES		0x35

//#define	CLASS_CAPABILITY_AUDIO							0x10
//#define	CLASS_CAPABILITY_DATA							0x20

 /* bearer transfer mode */
#define	BEARER_INFO_MODE_CIRCUIT									0x0
#define	BEARER_INFO_MODE_PACKET									0x2

#define 	BEARER_INFO_RATE_PACKET									0x00
#define 	BEARER_INFO_RATE_64KPS									0x10
#define 	BEARER_INFO_RATE_384KPS									0x13
#define 	BEARER_INFO_RATE_1536KPS									0x15

#define 	BEARER_INFO_L1_PROTOCOL_X30								0x01
#define 	BEARER_INFO_L1_PROTOCOL_G711_ULAW						0x02
#define 	BEARER_INFO_L1_PROTOCOL_X31								0x09

typedef enum
{ /* isdnsignal */
	ISDNSIGNAL_VOLUME,
	ISDNSIGNAL_CONF,
	ISDNSIGNAL_NODATA,		/* no data required */
	ISDNSIGNAL_ECHO,
}isdn_signal_t;


/* 5 bytes for this IE
*  0X04(MT), 0x03(length), 0x90(TTC+3.1KHz audio ),0x90(circuit, 64 kps),0xa2(bit4-7 must be A, 2 : ulaw )
*/

class IeBearer : public IsdnIE
{
	public:
		IeBearer(int codec, int capacity, int mode);

		IeBearer(const unsigned char *p)  throw(IsdnIeParserException &) ;
		
		IeBearer( const IeBearer& src);

		IeBearer( );
		
		const IeBearer& operator = (const IeBearer& src);

		bool operator ==( const IeBearer& src) const;

		IeBearer( const Data& srcData );

		Data getContentCoding() const;

		void setContentCoding(const Data& srcContentCoding);

		int getRate()
		{
			return rate;
		};
		
		void setRate(int _rate);

		int getMulti()
		{
			return multi;
		};
		
		void setMulti(int _multi);

		int getUser()
		{
			return user;
		};
		
		void setUser(int _user);

		IeBearer* duplicate() const;

		virtual bool compareIsdnIE(IsdnIE* msg) const;
		
		unsigned char *encode( msg_t *msg, int ntMode);

		int decode(const unsigned char *p);
		
		virtual int getIeType() const
		{
			return ISDN_IE_BEARER;
		};
		
#if WITH_DEBUG
		virtual const char *debugInfo();
#endif
		
	protected:	

	private:
		
		/* primary attributes , refer to vol 3 p.74 */
		/* coding standard, always 0 : ITU-T and TTC standard, bit 5-6 in 1 byte */
		int codec;
		/* information transfer cap,  bit0~4 in 1 byte */
		int capability;

		/* transfer bit 5-6, 00B: circuit, 10 : packet */
		int mode;/* circuit/packet */
		/* bit 0-4, 0x10, 64  kbps */
		int rate ;

		/* not provide in INS */
		int multi ;
		
		/* user info layer 1 protocol */
		int user ;

		int validateCodec(int _codec);
		int validateCapabilty(int _cap);
		int validateMode(int _mode);

		int validateRate(int _rate);
		int validateMulti(int _multi);
		int validateUser(int _user);

};
 
}

#endif

