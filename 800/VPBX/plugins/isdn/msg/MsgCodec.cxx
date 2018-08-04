/*
* $Id: MsgCodec.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "MsgCodec.hxx"

using namespace Assist;

MsgCodec::MsgCodec( )
{
}
		
MsgCodec::~MsgCodec()
{

}

ISDN_METHOD MsgCodec::getType(int prim)
{
	ISDN_METHOD type = ISDN_TYPE_UNKNOWN;
	
	switch ( prim)
	{
//		case CC_NEW_CR | INDICATION:
//			return ISDN_TYPE_CC_NEW;

		case CC_SETUP | CONFIRM:/* nt-library now gives us a new id via CC_SETUP_CONFIRM */
		case CC_SETUP | INDICATION:
		case CC_SETUP | REQUEST:
			return ISDN_TYPE_SETUP;

		case CC_INFORMATION | INDICATION:
			return ISDN_TYPE_INFO;

		case CC_SETUP_ACKNOWLEDGE | INDICATION:
		case CC_SETUP_ACKNOWLEDGE | REQUEST:
			return ISDN_TYPE_SETUP_ACK;

		case CC_PROCEEDING | INDICATION:
		case CC_PROCEEDING | REQUEST:
			return ISDN_TYPE_PROCEEDING;

		case CC_ALERTING | INDICATION:
		case CC_ALERTING | REQUEST:
			return ISDN_TYPE_ALERTING;

		case CC_CONNECT | INDICATION:
		case CC_CONNECT | REQUEST:
			return ISDN_TYPE_CONNECT;

		case CC_CONNECT_ACKNOWLEDGE | INDICATION:
		case CC_CONNECT | CONFIRM:
			return ISDN_TYPE_CONNECT_ACK;

		case CC_DISCONNECT | INDICATION:
		case CC_DISCONNECT | REQUEST:
			return ISDN_TYPE_DISCONNECT;

		case CC_RELEASE | INDICATION:
		case CC_RELEASE | CONFIRM:
		case CC_RELEASE | REQUEST:
		case CC_RELEASE | RESPONSE:
			return ISDN_TYPE_RELEASE;

		case CC_RELEASE_COMPLETE | INDICATION:
		case CC_RELEASE_COMPLETE | RESPONSE:
			return ISDN_TYPE_RELEASE_COMPLETE_IND;

		case CC_RELEASE_COMPLETE | CONFIRM:
		case CC_RELEASE_COMPLETE | REQUEST:
			return ISDN_TYPE_RELEASE_COMPLETE_CFM;

		case CC_NOTIFY | REQUEST:
		case CC_NOTIFY | INDICATION:
			return ISDN_TYPE_NOTIFY;

		case CC_HOLD | INDICATION:
		case CC_HOLD | REQUEST:
			return ISDN_TYPE_HOLD;

		case CC_RETRIEVE | REQUEST:
		case CC_RETRIEVE | INDICATION:
			return ISDN_TYPE_RETRIEVE;

		case CC_SUSPEND | REQUEST:
		case CC_SUSPEND | INDICATION:
			return ISDN_TYPE_SUSPEND;

		case CC_RESUME | REQUEST:
		case CC_RESUME | INDICATION:
			return ISDN_TYPE_RESUME;

		case CC_FACILITY | REQUEST:
		case CC_FACILITY | INDICATION:
			return ISDN_TYPE_FACILITY;

		case CC_RELEASE_CR | REQUEST:/* release Call Ref */
		case CC_RELEASE_CR | INDICATION:/* release Call Ref */
		case CC_RELEASE_CR | CONFIRM:/* release Call Ref */
			return ISDN_TYPE_RELEASE_CR;

		case CC_NEW_CR | REQUEST:	/* new Call Ref */
		case CC_NEW_CR | INDICATION:	/* new Call Ref */
			return ISDN_TYPE_NEW_CR;
			
		case CC_TIMEOUT | REQUEST:	
		case CC_TIMEOUT | INDICATION:
			return ISDN_TYPE_CC_TIMEOUT;

	}

	return type;
}

unsigned char *MsgCodec::getL3Data(const msg_t *msg)
{
	unsigned char *p = (unsigned char *)msg->data;
	p += headerLength;
	return p;
}

