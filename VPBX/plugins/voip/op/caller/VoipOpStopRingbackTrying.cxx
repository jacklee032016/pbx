/*
* $Id: VoipOpStopRingbackTrying.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/
#include "VoipOpBuilder.hxx"

using namespace Assist;

const Sptr <PbxState> VoipOpStopRingbackTrying::process( const Sptr <PbxEvent> event )
{
	Sptr <SipContext> context = getSipContext(event);
	assert( context != 0 );

	bool bRing = false;
	//added by lijie
	bool bCWing = false;

	Sptr <Contact> contact;
	Sptr <ContactList> contacts = context->getContactList();
	ContactList::iterator iter = contacts->begin();

	while ( iter != contacts->end() )
	{
		contact = (*iter);
		assert( contact != 0 );
		if ( contact->getStatus() == SIP_STATUS_CODE_INFO_RINGING || 
			contact->getStatus() == SIP_STATUS_CODE_SUCCESS_OK || 
			contact->getStatus() == SIP_STATUS_CODE_INFO_PROGRESS)
		{
			bRing = true;
			//added by lijie
			if(contact->getStatus() == SIP_STATUS_CODE_INFO_QUEUED)
				bCWing = true;
			break;
		}
		iter++;
	}

	if(bRing)
	{
		Sptr <VoipEndPoint> vep = getVoipEndPoint(event);	
#if 0
			//added by lijie	
		if(!bCWing)
       		signal->signalOrRequest.signal = DeviceSignalLocalRingbackStop;
		else
		 	signal->signalOrRequest.signal = DeviceSignalStopCallWaitingBeep;
#endif		
//		vep->audioStop();
	}

	return PBX_STATE_CONTINUE;
}

