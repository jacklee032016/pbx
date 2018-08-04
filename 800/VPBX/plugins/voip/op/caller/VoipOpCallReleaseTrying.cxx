/*
* $Id: VoipOpCallReleaseTrying.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipOpBuilder.hxx"

#include "CancelMsg.hxx"

using namespace Assist;

/* process _CallRelease and Timeout when in state of TRYING */
const Sptr <PbxState> VoipOpCallReleaseTrying::process( Sptr <PbxEvent> event )
{
	if(checkCallMsg(event) != CALL_RELEASE && !isTimeout(event) )
	{
		return PBX_STATE_CONTINUE;
	}

	Sptr <SipContext> context =getSipContext(event);
	assert( context != 0 );

	Sptr <Contact> contact;
	Sptr <ContactList> contacts = context->getContactList();

	ContactList::iterator iter = contacts->begin();
	while ( iter != contacts->end() )
	{
		contact = (*iter);
		assert( contact != 0 );
		if ( contact->getStatus() < 200 )
		{
			cpDebug(LOG_DEBUG, "CANCEL contact" );
			CancelMsg msg( contact->getInviteMsg() );
			getVoipEndPoint(event)->getSipStack()->sendAsync( msg );
		}
		iter++;
	}

	return lookupEPState(event, VOIP_EP_STATE_CANCEL_TRYING);
}

