#ifndef UserAgent_Hxx
#define UserAgent_Hxx

/*
 * $Id: UserAgent.hxx,v 1.3 2007/03/28 19:03:39 lizhijie Exp $
 */

#include <Sptr.hxx>
#include <HeartLessProxy.hxx>

#include "DeviceThread.hxx"
#include "AssistPbxRtpThread.hxx"
#include "RtpThread.hxx"
#include "FeatureThread.hxx"

#if	__CDR_OPEN__
#include "RecordThread.hxx"
#endif
#include "UaBuilder.hxx"
#if 0
#include "LoadGenThread.hxx"
#endif

/*add by Tangbiao 2005-4-5*/
#include <signal.h>
#include <sys/types.h>
#include "UaConfiguration.hxx"
/*add by Tangbiao 2005-4-5*/


namespace Vocal
{
///
class UserAgent : public HeartLessProxy
{
    public:
        ///
        UserAgent( Sptr < UaBuilder > uaBuilder, 
                   unsigned short sipPort,
                   Data appName = "Assist SIP UserAgent" );

        ///
        void run();
        ///
        void join();
	 static void exit();

        ///
        ~UserAgent();

    protected:
		Sptr < DeviceThread > deviceThread;

		Sptr < AssistPbxRtpThread > pbxRtpThread;

		Sptr < RtpThread > rtpThread;

		Sptr < FeatureThread > featureThread;

#if	__CDR_OPEN__
		Sptr < RecordThread > recordThread;
#endif

#if 0        
		Sptr < LoadGenThread > loadGenThread;
#endif
};
 
}

#endif
