#ifndef SipReceive_HXX
#define SipReceive_HXX
/*
 * $Id: SipReceive.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "Sptr.hxx"

namespace Vocal
{

class SipMsg;

class SipReceive
{
    public:
        SipReceive();
        ~SipReceive();
        ///
        Sptr < SipReceive > getPrevMsg();
        ///
        Sptr < SipMsg > getCurMsg();
        ///
        void setPrevMsg(Sptr < SipReceive > );
        ///
        void setCurMsg(Sptr < SipMsg > );

    private:
        Sptr < SipReceive > prevMsg;
        Sptr < SipMsg > curMsg;
};
 
} // namespace Vocal

#endif
