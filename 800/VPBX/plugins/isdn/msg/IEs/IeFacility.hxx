#ifndef __IE_FACILITY_HXX__
#define __IE_FACILITY_HXX__
/*
* $Id: IeFacility.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"

#include "IeContent.hxx"
#include "VException.hxx"

namespace Assist
{

/* component type in FACILITY */
#define FACILITY_INVOKE										0xA1
#define FACILITY_RETURN_RESULT								0xA2
#define FACILITY_RETURN_ERROR								0xA3
#define FACILITY_REJECT										0xA4


#define INVOKE_IDEN_TAG									0x02

#define SUPPLEMENTARY_SERVICE								0x11

#define FACILITY_LENGTH										0x14
#define FACILITY_INVOKE_LENGTH								0x11

#define FACILITY_ID_INTERGER								0x02
#define FACILITY_ID_LENGTH									0x01
#define FACILITY_ID_VALUE									0x0A

#define FACILITY_OPERATION_OBJECT							0x06
#define FACILITY_OPERATION_LENGTH							0x07

#define FACILITY_OPERATION_OID_CCITT						0x00
#define FACILITY_OPERATION_OID_NETWORK					0x03

#define FACILITY_OPERATION_OID_NTT_FIRST					0xA2
#define FACILITY_OPERATION_OID_NTT_SECOND				0x31

#define FACILITY_OPERATION_OID_ISDN_SERVICE_FIRST		0x87
#define FACILITY_OPERATION_OID_ISDN_SERVICE_SECOND		0x69

#define FACILITY_OPERATION_OID_OPERATION					0x01

#define FACILITY_OPERATION_OID_UNKNOWN					0x05


#define FACILITY_SET_TYPE									0x31
#define FACILITY_SET_LENGTH								0x03



/*
	unsigned char facility[32] = 
		{ 
			0x1c, 0x14, 0x91, 0xa1, 0x11,
			0x02, 0x01, 0x0a, 0x06, 0x07,
			0x03, 0xa2, 0x31, 0x87, 0x69,
			0x01, 0x05, 0x31, 0x03, 0x81,
			0x01, 0x01 
		};
* refer to p.141 of vol.4 
	byte 0 : IE_id          = 0x1C;  
	byte 1 : length        = 0x14;  
		byte 2 : 			 = 0x91
			extension_1    = 1;  	//bit 7
			spare          = 0x00;  // bit 5-6
			prot_profile   = 0x11; // Supplementary Service (ROSE)  bit 0-4,so byte 3 is 0x91
			byte 3 : comp_type      = 0xA1; // Invoke  
			byte 4 : comp_length    = 0x11;   // Component Length (Data Only)  
				byte 5 : invoke ID type = 0x02; // Invoke Identifier, tag , interger type 
				byte 6 : Invoke ID length   = 0x01; // Invoke Identifier, length  
				byte 7 : Invoke ID value   = 0x0A; // Invoke Identifier, invoke ie (varies)  
				
				byte 8 : Operation Type   = 0x06; // Operation Object ID
				byte 9 : Operation length   = 0x07; // Operation Object, length  
					Operation Content:
					byte 10 : CCITT(0)+NETWORK-OPERATOR(3)  =0x03; 				// Operation Object, Operation Value  
					byte 11,12 : NTT(4401) =0xa2,0x31   								// Operation Object, Operation Value  
					byte 13,14 : ISDN-NETWORKSPECIFIC_SERVICE(1001)  = 0x87, 0x69; 	// Operation Object, Operation Value
					byte 15 : OPERATION = 0x01 									// Operation Object, Operation Value 
					byte 16 : =0x05		???
					
				byte 17 :   = 0x31; // Operation Object, Operation Value  
				byte 18 :  length  = 0x03; // Operation Object, Operation Value  
				byte 19 :  = 0x81; // Operation Object, Operation Value  
				byte 20 :  = 0x01; // Sequence, tag  
				byte 21 :  = 0x01

*/

/*
* IE type 0x1c, not defined in INS( this IE is not NET-FACILITY  
* variable length IE: MT, length, content(IA5 chars) 2005.12.03
*/

/*
*  IE Facility must be used in ISDN PBX environment, although it is not offered in INS network 
*  It is only used in SETUP msg 
*/
class IeFacility : public IeContent
{
	public:
		IeFacility( const unsigned char *p) throw(IsdnIeParserException &) ;
		IeFacility( );
		IeFacility(const IeFacility& src);
		~IeFacility( );

		const IeFacility& operator=(const IeFacility& src);
		bool operator == (const IeFacility& other) const;
		bool operator != (const IeFacility& other) const
			{ return !(*this == other);}

		bool operator < (const IeFacility& other) const;
		bool operator > (const IeFacility& other) const;
		IsdnIE* duplicate() const;
		virtual bool compareIsdnIE(IsdnIE* msg) const;
		
		unsigned char *encode( msg_t *msg, int ntMode);
		
		virtual int getIeType() const
		{
			return ISDN_IE_FACILITY;
		};
		

#if WITH_DEBUG
		virtual const char *debugInfo();
#endif

	protected:
		
	private:
};
 
}

#endif

