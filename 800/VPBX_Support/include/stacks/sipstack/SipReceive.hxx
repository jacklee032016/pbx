#ifndef SipReceive_HXX
#define SipReceive_HXX
/*
* $Log: SipReceive.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SipReceive.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "Sptr.hxx"

namespace Assist
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
 
}

#endif
