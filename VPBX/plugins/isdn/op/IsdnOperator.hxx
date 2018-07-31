#ifndef __ISDN_OPERATOR_HXX__
#define __ISDN_OPERATOR_HXX__

/*
 * $Id: IsdnOperator.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "EpOperator.hxx"

#include "IsdnEndPoint.hxx"
#include "IsdnEvent.hxx"
#include "IsdnMsg.hxx"
#include "IsdnCtrlDevice.hxx"

namespace Assist
{

class IsdnOperator : public EpOperator
{
	public:
		IsdnOperator(const string& name);
		virtual ~IsdnOperator();

	protected:

		virtual IsdnCtrlDevice *getIsdnCtrlDevice( const Sptr <PbxEvent> event );

		virtual Sptr <IsdnMsg> isIsdnMsg( const Sptr <PbxEvent> event );

		virtual ISDN_METHOD checkIsdnMsgType( const Sptr <PbxEvent> event );

	private:
		IsdnOperator(const IsdnOperator& rhs);
		IsdnOperator& operator=(const IsdnOperator& rhs);
};
 
}

#endif

