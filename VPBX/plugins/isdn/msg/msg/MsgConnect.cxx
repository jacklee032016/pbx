/*
* $Id: MsgConnect.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "global.h"
#include "cpLog.h"

#include "MsgConnect.hxx"

#include "IeChanId.hxx"
#include "IeProgress.hxx"
#include "IeDisplay.hxx"
#include "IeSignal.hxx"

#ifdef CENTREX
#include "IeCentrex.hxx"
#endif

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "MsgEncoder.hxx"

#include "IsdnEndPoint.hxx"
#include "IsdnCtrlDevice.hxx"

using namespace Assist;

MsgConnect::MsgConnect(const msg_t *msg, IsdnCtrlDevice *ctrl)  throw(IsdnMsgParserException &) 
        : IsdnMsg(msg, ctrl)
{
#if 0
	if( getType() !=  myCtrl->getDecoder()->getType(msg) )
	{
		cpLog(LOG_DEBUG_STACK, "Failed in parse IsdnMsg (prim :0x%x) on device %s", ctrl->getDecoder()->getPrim, ctrl->getName() );
		throw  IsdnMsgParserException("failed in decode ISDN MSG", __FILE__, __LINE__, res );
	}
#endif
	unsigned char *data = myCtrl->getDecoder()->getL3Data( msg);

	try
	{
		decode(data);
	}
	catch (IsdnMsgParserException&)
	{
		if (IsdnParserMode::isdnParserMode())
		{
			throw  IsdnMsgParserException("failed in decode ISDN msg", __FILE__, __LINE__, ERROR_MSG_IS_NULL );
		}
	}
}

MsgConnect::MsgConnect( IsdnCtrlDevice *ctrl)
        : IsdnMsg(ctrl)
{
}

MsgConnect::MsgConnect( int _dlci, Sptr <IeChanId> chanId,IsdnCtrlDevice *ctrl)
	: IsdnMsg(ctrl)
{
	dlci = _dlci;
	ieChanId = chanId;
}

MsgConnect::MsgConnect(const MsgConnect& src)
        : IsdnMsg(src)
{
}

MsgConnect::~MsgConnect()
{
}

MsgConnect& MsgConnect::operator =(const MsgConnect& src)
{
	if ( &src != this)
	{
		*static_cast < IsdnMsg* > (this) = src;
		this->ieChanId = src.ieChanId;
//		this->ieConnectPN = src.ieConnectPN;
#ifdef CENTREX
		this->ieCentrex = src.ieCentrex;
#endif
	}
	return (*this);
}

bool MsgConnect::operator ==(const MsgConnect& src) 
{
	if(this->ieChanId==src.ieChanId )//&& this->ieConnectPN == src.ieConnectPN)
	{
#ifdef CENTREX
		if(this->ieCentrex==src.ieCentrex)
#endif
			return true;
	}
	return false;
}

ISDN_METHOD MsgConnect::getType() const
{
	return ISDN_TYPE_CONNECT;
}

#if 0
void MsgConnect::setConnectPN(Sptr <IeConnectPN> connectPn)
{
	ieConnectPN = connectPn;
}

Sptr <IeConnectPN> MsgConnect::getConnectPN()
{
	return ieConnectPN;
}
#endif

void MsgConnect::setChanId(Sptr <IeChanId> chanId)
{
	ieChanId= chanId;
}

Sptr <IeChanId> MsgConnect::getChanId()
{
	return ieChanId;
}

#ifdef CENTREX
void MsgConnect::setCentrex(Sptr <IeCentrex> centrex)
{
	ieCentrex = centrex;
}

Sptr <IeCentrex> MsgConnect::getCentrex()
{
	return ieCentrex;
}
#endif

int MsgConnect::decode(const unsigned char *iesData)  throw(IsdnMsgParserException &)
{
	const unsigned char *p;

	try
	{
/*
	CONNECT_t *conn = (CONNECT_t *)((unsigned long)data + headerlen);
	if (ntMode)
		parent->DLCI = conn->ces;
*/
	/* NOTE: we do not check the connected channel, since we
	 * ready sent a channel to the remote side	 */
	/* channel id */

		p = myCtrl->getDecoder()->getChanId4ConnectMsg(iesData);
		if(p != NULL)
			ieChanId = new IeChanId( p);
#if 0		
		p = myCtrl->getDecoder()->getConnectPN4ConnectMsg(iesData);
		ieConnectPN = new IeConnectPN( p);
#endif		
#ifdef CENTREX
		/* te-mode: CONP (connected name identification presentation) */
		if (!ntMode)
		{
			
			p = myCtrl->getDecoder()->getCentrex4ConnectMsg(iesData);
			if(p != NULL)
				ieCentrex = new IeProgress( p);
		}
#endif
		dlci = 0;
		
	}
	catch (IsdnIeParserException&)
	{
//		throw  IsdnMsgParserException("failed in decode CONNECT msg", __FILE__, __LINE__, ERROR_MSG_IS_NULL );
		cpLog(LOG_DEBUG, "failed in decode CONNECT msg" );
	}
	
	return AS_OK;
}

#if WITH_DEBUG
void MsgConnect::debugInfo()
{
	const char *p;
	cpLog(LOG_DEBUG, "L3 MSG : CONNECT");
	
	IsdnMsg::debugInfo();

	cpLog(LOG_DEBUG, "\t dlci \t: %d" ,  dlci );
	if(ieChanId != 0)
	{
		p = ieChanId->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}	

	if(ieProgress != 0 )
	{	
		p = ieProgress->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}

	if(ieDisplay != 0 )
	{	
		p = ieDisplay->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}

	if(ieSignal != 0 )
	{	
		p = ieSignal->debugInfo();
		cpLog(LOG_DEBUG, "\t %s" , p );
		free((void *)p);
	}

#ifdef CENTREX
	p =  ieCentrex->debugInfo();
	cpLog(LOG_DEBUG, "\t %s" ,p );
	free((void *)p);
#endif
}
#endif

int MsgConnect::getMsgType() const
{
	return MT_CONNECT;
}

int MsgConnect::getCCType() const
{
	return CC_CONNECT;
}

void MsgConnect::encode( msg_t *msg)
{
	assert(msg != 0);
	unsigned char *header = myCtrl->getEncoder()->getL3Data(msg);
	int 		ntMode = myCtrl->getNtMode();

	if(ieChanId != 0)
	{
		myCtrl->getEncoder()->encChanId4ConnectMsg(header, ieChanId->encode( msg, ntMode) );
	}
	if( ieProgress != 0)
	{
		myCtrl->getEncoder()->encProgress4ConnectMsg( header, ieProgress->encode( msg, ntMode) );
	}

	/* following only used in n-->u */
	if( ieDisplay != 0)
	{
		myCtrl->getEncoder()->encDisplay4ConnectMsg( header, ieDisplay->encode( msg, ntMode) );
	}
	if( ieSignal != 0)
	{
		myCtrl->getEncoder()->encSignal4ConnectMsg(header, ieSignal->encode( msg, ntMode) );
	}

	return ;
}

const char *MsgConnect::getName()
{
	return "ISDN-CONNECT";
}

