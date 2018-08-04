/*
* $Id: IsdnIE.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "IsdnIE.hxx"

using namespace Assist;

struct IsdnIeTableEntry
{
	IsdnIeType type;
	int iEid;					/* identify defined in Q.931 */
	const char * name;
};

// This table should be maintained in alphabetical order.  The entry
// you add needs to be in the exact same place as the entry in
// IsdnIeType in IsdnIE.hxx.  

static const IsdnIeTableEntry isdnIeTable[isdnIeCount] =
{
	{ ISDN_IE_SEGMENT, 			0x00,	"Segment" },
	{ ISDN_IE_BEARER, 			0x04,	"Bearer" },
	{ ISDN_IE_CAUSE, 			0x08,	"Cause" },
	{ ISDN_IE_CALL_ID, 			0x10,	"CallId" },
	{ ISDN_IE_CALL_STATE,		0x14,	"CallState" },
	{ ISDN_IE_CHANNEL_ID, 		0x18,	"ChannelId" },
	{ ISDN_IE_FACILITY, 			0x1c,	"Facility" },
	{ ISDN_IE_PROGRESS,			0x1e,	"Progress" },
	{ ISDN_IE_NET_FAC, 			0x20,	"NetFacility" },
	{ ISDN_IE_NOTIFY, 			0x27,	"Notify" },
	{ ISDN_IE_DISPLAY,			0x28,	"Display" },
	{ ISDN_IE_DATE, 				0x29,	"Date" },
	{ ISDN_IE_KEYPAD, 			0x2c,	"KeyPad" },
	{ ISDN_IE_SIGNAL, 			0x34,	"Signal" },
	{ ISDN_IE_INFORATE, 			0x40,	"InfoRate" },
	{ ISDN_IE_E2E_TDELAY,		0x42,	"Delay" },
	{ ISDN_IE_PACK_BINPARA,		0x43,	"PacketParam" },
	{ ISDN_IE_PACK_WINSIZE, 		0x44,	"PacketWinSize" },
	{ ISDN_IE_PACK_SIZE, 		0x45,	"PacketSize" },

	{ ISDN_IE_CUG,          			0x47,	"Cug" },
	{ ISDN_IE_REV_CHARGE,          	0x4a,	"Charge" },
	{ ISDN_IE_CONNECT_PN,          	0x4c,	"ConnectPN" },
	{ ISDN_IE_CONNECT_SUB,		0x4d,	"ConnectSub" },
	{ ISDN_IE_CALLING_PN,          	0x6c,	"CallingPN" },
	{ ISDN_IE_CALLING_SUB,          	0x6d,	"CallingSub" },
	{ ISDN_IE_CALLED_PN,          	0x70,	"CalledPN" },
	{ ISDN_IE_CALLED_SUB,          	0x71,	"CalledSub" },
	{ ISDN_IE_REDIR_NR,          	0x74,	"RedirNR" },
	{ ISDN_IE_REDIR_DN,          	0x76,	"RedirDN" },
	{ ISDN_IE_TRANS_SEL,          	0x78,	"TransSelect" },
	{ ISDN_IE_RESTART_IND,          	0x79,	"RestartIndication" },
	{ ISDN_IE_LLC,          			0x7c,	"LLC" },
	{ ISDN_IE_HLC,          			0x7d,	"HLC" },
	{ ISDN_IE_USER_USER,		0x7e,	"UserUser" },
	{ ISDN_IE_ESCAPE,			0x7f,	"Escape" },
	{ ISDN_IE_CNIP, 				0x80,	"CNIP" },
	{ ISDN_IE_SHIFT, 				0x90,	"Shift" },
	{ ISDN_IE_MORE_DATA, 		0xa0,	"MoreData" },
	{ ISDN_IE_COMPLETE, 			0xa1,	"Complete" },
	{ ISDN_IE_CONGESTION,		0xb0,	"Congestion" },
	{ ISDN_IE_REPEAT, 			0xd0,	"Repeat" },
	{ ISDN_IE_CENTREX_FAC,          0x88,	"CentrixFacility" },
	{ ISDN_IE_CENTREX_ID,		0xa1,	"CentrixId" },

	{ ISDN_IE_UNKNOWN,			0xff,	""} 
};

IsdnIeParserException::IsdnIeParserException( const string& msg, const string& file,  const int line,  const int error )
	: VException( msg, file, line,  error),
	value(error)
{
}


string IsdnIeParserException::getName( void ) const
{
	return "ISDN IE ParserException";
}

IsdnIE::IsdnIE(void *data, int ntMode)
{
	emptyFlg= true;
}

IsdnIE::IsdnIE()
{
	emptyFlg= true;
}

IsdnIE::IsdnIE(const IsdnIE& src)
{
	emptyFlg= src.emptyFlg;
}

IsdnIE::IsdnIE(const unsigned char *p ) throw(IsdnIeParserException &)
{
#if 0
	int res = decode(p);
	if( res != AS_OK)
	{
		throw  IsdnIeParserException("failed in Decode Cause IE", __FILE__, __LINE__, res );
	}
#endif
}

IsdnIE::~IsdnIE()
{
}

bool IsdnIE::isEmpty() const 
{ 
	return emptyFlg; 
}

