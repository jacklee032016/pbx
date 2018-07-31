#ifndef __IE_USER_USER_HXX__
#define __IE_USER_USER_HXX__
/*
* $Id: IeUserUser.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "IeContent.hxx"

#include "VException.hxx"

namespace Assist
{

#define IE_USERUSER_PROTOCOL_USER							0x00
#define IE_USERUSER_PROTOCOL_OSI_HIGH					0x01
#define IE_USERUSER_PROTOCOL_X244							0x02
#define IE_USERUSER_PROTOCOL_SYSTEM_MANAGE				0x03
#define IE_USERUSER_PROTOCOL_IA5_CHARS					0x04
#define IE_USERUSER_PROTOCOL_X209							0x05
#define IE_USERUSER_PROTOCOL_V120							0x07
#define IE_USERUSER_PROTOCOL_Q931_USER2NET				0x08

#define IE_USERUSER_PROTOCOL_NATIONAL					0x40


/*
* p.134
* variable length IE: MT, length, protocol ID; user Info
*/
class IeUserUser : public IeContent
{
	public:
		IeUserUser( const unsigned char *p)  throw(IsdnIeParserException &) ;
		IeUserUser( int _protocol, const unsigned char *_content);
		IeUserUser(const IeUserUser&);
		~IeUserUser( );
		
		IeUserUser& operator = (const IeUserUser& src);

		bool operator ==(const IeUserUser& src) const;
		
		const int getProtocol() const
		{
			return protocol;
		};
		
		const unsigned char *getContent() const
		{
			return content;
		};
		
		const int getLength() const
		{
			return length;
		};

		/// method for copying sip headers of any type without knowing which type
		IsdnIE* duplicate() const;

		/// compare two headers of (possibly) the same class
		virtual bool compareIsdnIE(IsdnIE* msg) const;
	
		unsigned char *encode(msg_t *msg, int ntMode);

		int decode(const unsigned char *p);
		
		virtual int getIeType() const
		{
			return ISDN_IE_USER_USER;
		};
		

#if WITH_DEBUG
		virtual const char *debugInfo();
#endif
	protected:

	private:
		int 		protocol;
		
		
};
 
}
#endif

