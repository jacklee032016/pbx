#ifndef OPTIONSMSG_H
#define OPTIONSMSG_H
/*
* $Log: OptionsMsg.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: OptionsMsg.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "SipCommand.hxx"

namespace Assist
{

class SipUrl;

///
class OptionsMsg : public SipCommand
{
    public:
        ///
        enum OptionsMsgForward { optionsMsgForward };
        ///
        OptionsMsg();

        ///
        virtual ~OptionsMsg(){};
        //The listenPort is required to build the INVITE msg, in the Via field.
        OptionsMsg(Sptr <BaseUrl> url, int listenPort = 5060, int rtpPort = 3456);  // TODO fix rtpPort default

        /// this should not be called by anyone but the SipMsg class
        OptionsMsg( const Data& data );
        ///
        OptionsMsg& operator =(const OptionsMsg&);
        ///
        bool  operator ==(const OptionsMsg& src); 
        ///
        OptionsMsg(const OptionsMsg&);

        ///
        OptionsMsg(const OptionsMsg&, enum OptionsMsgForward);

        ///
//        virtual Data encode() const;  // get details and create Message, and return.
        ///
        virtual Method getType() const;

    private:
        void setOptionsDetails(Sptr <BaseUrl> url, int listenPort, int rtpPort);
};
 
}

#endif
