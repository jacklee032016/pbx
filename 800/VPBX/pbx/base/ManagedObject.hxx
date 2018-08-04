#ifndef __MANAGED_OBJECT_HXX__
#define __MANAGED_OBJECT_HXX__
/*
* $Id: ManagedObject.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "VMutex.h"
#include "Sptr.hxx"
#include "cpLog.h"

namespace Assist 
{

class PbxManager;
class PbxTimerEvent;


/* Object with state and timer in our vpbx environments, such as EndPoint, CallInfo and DigitCollector */
class ManagedObject 
{
	public:
		ManagedObject(Sptr <PbxManager> parent );
		virtual ~ManagedObject();

#if 0		
		/** Timer mutator*/
		void setTimer(const Sptr <PbxTimerEvent> timerEvent);

		Sptr < PbxTimerEvent > getTimer() const;

		/** Sets itsTimer member to 0.*/
		void removeTimer();
#endif
		Sptr <PbxManager>  getParent();
		
	protected:


	private:
//		ManagedObject( );

		Sptr <PbxManager>			myParent;
		
		Sptr <PbxTimerEvent> 			myTimer;

};

}

#endif

