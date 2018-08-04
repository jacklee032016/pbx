#ifndef __EP_STATE_HXX__
#define __EP_STATE_HXX__
/*
 * $Id: EpState.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "EpOperator.hxx"
#include "EndPoint.hxx"
#include "PbxState.hxx"

namespace Assist 
{

/*
* We have circular dependancies, so forward declare.
 */

class EpState;

/* common super class for all EndPoint State in ISDN/PSDN/SIP/Console */
class EpState : public PbxState
{
	public:
		EpState(const string& name, int state = PBX_STATE_INVALIDATE_ID);
		virtual ~EpState();

	protected:
		virtual Sptr <PbxStateObject>  getMyStateObject(const Sptr <PbxEvent> event) ;
		virtual void  setMyStateObject(const Sptr <PbxEvent> event, Sptr <PbxStateObject> obj) ;

		virtual void initState() = 0;
		
	private:
		EpState(const EpState &);
		const EpState & operator=(const EpState &);
};

#define ENDPOINT_STATE_DECLARE(type, name, state)  \
class type : public EpState\
{ \
public: \
	type() : EpState(name, state) { initState();}; \
	~type(){}; \
	virtual void  initState(); \
}

}

#endif 

