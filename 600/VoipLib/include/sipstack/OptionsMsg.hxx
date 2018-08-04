#ifndef OPTIONSMSG_H
#define OPTIONSMSG_H
/*
 * $Id: OptionsMsg.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */


#include "SipCommand.hxx"


namespace Vocal
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
 
} // namespace Vocal

#endif
