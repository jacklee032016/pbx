/*
 * $Id: ConsoleTask.cxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
 */

#include "ConsoleAudioChannel.hxx"
#include "EndPoint.hxx"

#include "assist_lib.h"
#include "PbxConfiguration.hxx"

using namespace Assist;

/* decode DTMF digit for ConsoleAudioChannel */
void *console_decode_task(void *param)
{
	char phoneNumber[128];
	int res;
	int i;
	
	ConsoleAudioChannel *audio = static_cast<ConsoleAudioChannel *>(param); 
	assert(audio !=0 );
	assert(audio->myDsp != 0);

	Sptr <EndPoint> ep = audio->getEndPoint();
	assert(ep != 0);
	
	cpDebug(LOG_DEBUG, "Console DTMF Detect thread running!" );

	while( audio->isDecoding )
	{
#if 0
		if(audio->dtmfBufInIndex != audio->dtmfBufOutIndex && audio->dtmfBufOutIndex!= -1)//||
//			( audio->dtmfBufInIndex==0 ) )
#else
		if(audio->dtmfBufInIndex > audio->dtmfBufOutIndex )
		
#endif
		{
			
			audio->dtmfBufOutIndex = (audio->dtmfBufOutIndex+1)%audio->dtmfBufCount;
			cpDebug(LOG_DEBUG, "Decoding Thread work on %d buffer\n", audio->dtmfBufOutIndex);
			res = as_dsp_dtmf_console_decode(audio->myDsp, 
				(unsigned char *)audio->dtmfBufs+audio->dtmfBufOutIndex*audio->dtmfBufSize, 
				audio->dtmfBufSize, phoneNumber, sizeof(phoneNumber) );
			if(res > 0 )
			{
				for(i=0; i< res; i++)
				{
					ep->genDtmfEvent( phoneNumber[i]);
				}
				cpDebug(LOG_DEBUG, "Console decode '%s' Digits\n\n", phoneNumber);
			}
		}
		if(audio->isDecoding == false)
		{
//			printf("Console DTMF Decoding thread break\n");
			break;
		}	
		
	}
	
	cpDebug(LOG_DEBUG, "Console DTMF Detect thread ended!" );
	return 0;
}


