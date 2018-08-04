#ifndef RegistrationManager_HXX
#define RegistrationManager_HXX
/*
 * $Id: RegistrationManager.hxx,v 1.5 2007/03/28 19:03:39 lizhijie Exp $
 */

#include <vector>
#include <map>

#include <Fifo.h>
#include "VThread.hxx"
#include <VMutex.h>
#include <StatusMsg.hxx>
#include "SipTransceiver.hxx"
#include "Registration.hxx"
#include "SipCallId.hxx"

namespace Vocal
{


class RegistrationManager
{
    public:
		RegistrationManager( Sptr <SipTransceiver> sipstack );
		~RegistrationManager();

		void startRegistration();
		void addRegistration(const Registration& item);

		/* added by lizhijie, 2005.02.17 */
		void  addRegistration(const string& phone_num, unsigned int cseq, bool register = true);
		/* end of added */

		//this function return false if the input StatusMsg is not
		//a response to a register message; otherwise, true is returned
		  int handleRegistrationResponse(const StatusMsg& msg);

		///
		void unRegister();

    private:
		
		typedef vector <Registration*>			RegistrationList;
		RegistrationList						registrationList;

		typedef map <Registration*, FifoEventId> FifoEventIdMap;
		FifoEventIdMap						fifoEventIdMap;

		Registration* findRegistration(const StatusMsg& msg);
		void flushRegistrationList();

		void registrationMain();
		VThread registrationThread;
		static void* registrationThreadWrapper(void* session);
		
		void						onceReg(bool reg=true); 
		Fifo <Registration *>		registrationFifo;

		VMutex					registrationMutex;

		Sptr <SipTransceiver>		sipStack;
		/* un-register flags */
		bool						urflag;			

		bool						exitFlag;

		/* map phone number and SipCallID */
		typedef map <string, Sptr <SipCallId> > TableCID;
		
		TableCID tableCallid;
};
 
}

#endif

