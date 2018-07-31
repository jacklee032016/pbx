/*
* $Id: MsgRetrieve.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

 
#include "global.h"
#include "cpLog.h"

#include "MsgRetrieve.hxx"

#include "IeChanId.hxx"

#include "IsdnParserMode.hxx"
#include "IsdnMsgFactory.hxx"
#include "MsgDecoder.hxx"
#include "MsgEncoder.hxx"

using namespace Assist;

MsgRetrieve::MsgRetrieve(const iframe_t *frm)   throw(IsdnMsgParserException &)
	:IsdnMsg( frm)
{
#if 0
	if( getType() !=  myCtrl->getDecoder()->getType(_msg) )
	{
		cpLog(LOG_DEBUG_STACK, "Failed in parse IsdnMsg (prim :0x%x) on device %s", ctrl->getDecoder()->getPrim, ctrl->getName() );
		throw  IsdnMsgParserException("failed in decode ISDN MSG", __FILE__, __LINE__, res );
	}
#endif
	unsigned char *data = myCtrl->getDecoder()->getL3Data( _msg);

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

MsgRetrieve::MsgRetrieve(const MsgRetrieve& src)
        : IsdnMsg (src)
{
}

MsgRetrieve::MsgRetrieve(Sptr <IeChanId> _chanId)
        : IsdnMsg()
{
	ieChanId = _chanId;
}


MsgRetrieve::MsgRetrieve( )
        : IsdnMsg()
{
}

MsgRetrieve::~MsgRetrieve( )
{
}


MsgRetrieve& MsgRetrieve::operator=( const MsgRetrieve& src )
{
	if ( &src != this )
	{
		IsdnMsg::operator=( src );
		this->ieChanId = src.ieChanId;
	}

	return ( *this );
}

bool MsgRetrieve::operator ==(const MsgRetrieve& src) 
{
	if( this->ieChanId == src.ieChanId)
		return true;
	return false;
}

ISDN_METHOD MsgRetrieve::getType() const
{
	return ISDN_TYPE_RETRIEVE;
}

Sptr <IeChanId> MsgRetrieve::getChanId() const
{
	return ieChanId;
}

void MsgRetrieve::setChanId(Sptr <IeChanId> chanId)
{
	ieChanId = chanId;
}


int MsgRetrieve::decode(const iframe_t *frm)  throw(IsdnMsgParserException &)
{
	const char *p = NULL;

	try
	{
		p = IsdnMsgFactory::instance()->getTeDecoder()->getChanId4RetrieveMsg( frm);
		if( p != NULL )
			ieChanId = new IeChanId( p);
	}
	catch (IsdnIeParserException&)
	{
		throw  IsdnMsgParserException("failed in decode RETRIEVE msg", __FILE__, __LINE__, ERROR_MSG_IS_NULL );
	}
	
	return AS_OK;
}

#if WITH_DEBUG
void MsgRetrieve::debugInfo()
{
	const char *p;
	cpLog(LOG_DEBUG, "L3 MSG : RETRIEVE");
	IsdnMsg::debugInfo();

	p = ieChanId->debugInfo();
	cpLog(LOG_DEBUG, "\t %s" , p );
	free((void *)p);
}
#endif

int MsgRetrieve::getMsgType() const
{
	return MT_RETRIEVE;
}

const char *MsgRetrieve::getName()
{
	return "ISDN-RETRIEVE";
}

