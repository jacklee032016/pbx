#ifndef __PSTN_OPERATOR_HXX__
#define __PSTN_OPERATOR_HXX__

/*
 * $Id: PstnOperator.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "EpOperator.hxx"

#include "PbxHwEvent.hxx"
#include "PstnEvent.hxx"
#include "PstnEndPoint.hxx"

namespace Assist
{

/* common super class for all Call Operations */
//class PstnOperator : public PbxOperator
class PstnOperator : public EpOperator
{
	public:
		PstnOperator(const string& name);
		virtual ~PstnOperator();

		Sptr < Fifo < Sptr <PbxEvent> > > getBackFifo() const { return myBackFifo; }
		//void setBackFifo( Sptr < Fifo < Sptr <PbxEvent> > >  newFifo);
//		virtual Sptr < Fifo < Sptr <PbxEvent> > > getTimerFifo();
		

	protected:
		Sptr <PstnEvent> isPstnEvent(Sptr <PbxEvent> event);
		DeviceEventType  getPstnEventType(Sptr <PbxEvent> event );

		void sendCommand(Sptr <PbxEvent> event, DeviceSignalType cmd);

#if 0	
		bool setTimer( const Sptr <PbxEvent> event, unsigned int timeout );

		bool cancelTimer( const Sptr <PbxEvent> event );
		bool isTimeout( const Sptr <PbxEvent> evnet );
#endif

	private:
		PstnOperator(const PstnOperator& rhs);
		PstnOperator& operator=(const PstnOperator& rhs);

	protected:
		static Sptr < Fifo < Sptr <PbxEvent> > >  myBackFifo;
		
};

#define PSTN_OP_DECLARE(type, name) \
class type : public PstnOperator\
{ \
public: \
	type() : PstnOperator(name) {} \
	~type(){} \
	virtual const Sptr <PbxState>  process(const Sptr <PbxEvent> event); \
}


}

#endif

