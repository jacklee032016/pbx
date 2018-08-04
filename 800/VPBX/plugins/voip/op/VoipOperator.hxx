#ifndef __VOIP_OPERATOR_HXX__
#define __VOIP_OPERATOR_HXX__

/*
 * $Id: VoipOperator.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "EpOperator.hxx"

#include "PbxHwEvent.hxx"
#include "SipEvent.hxx"
#include "VoipEndPoint.hxx"

namespace Assist
{

class VoipOperator : public EpOperator
{
	public:
		VoipOperator(const string& name);
		virtual ~VoipOperator();

	protected:
		Sptr <SipEvent> 	isSipEvent( const Sptr <PbxEvent> event );
		Sptr <SipMsg>  	isSipMsg( const Sptr <PbxEvent> event );
		Sptr <StatusMsg>  isStatusMsg( const Sptr <PbxEvent> event );
		Method  			getSipMsgType(Sptr <PbxEvent> event);

		Sptr <SipContext> 	getSipContext(Sptr <PbxEvent> event);
		
		Sptr <VoipEndPoint>  getVoipEndPoint(const Sptr <PbxEvent> event );
		
	private:
		VoipOperator(const VoipOperator& rhs);
		VoipOperator& operator=(const VoipOperator& rhs);

	protected:
		
};

#define VOIP_OP_DECLARE(type, name) \
class type : public VoipOperator\
{ \
public: \
	type() : VoipOperator(name) {} \
	~type(){} \
	virtual const Sptr <PbxState>  process(const Sptr <PbxEvent> event); \
}


}

#endif

