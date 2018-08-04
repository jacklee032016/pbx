#ifndef _SIP_PARSER_MODE_HXX_
#define _SIP_PARSER_MODE_HXX_
/*
* $Log: SipParserMode.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:26:29  lizhijie
* AS800 VPBX
*
* Revision 1.1  2006/06/03 12:06:38  lizhijie
* no message
*
* $Id: SipParserMode.hxx,v 1.1.1.1 2006/11/30 16:26:29 lizhijie Exp $
*/

#include "Data.hxx"

namespace Assist
{


class SipParserMode
{
    public:
        static bool sipParserMode()
        {
            if (!instance_)
            {
                instance_ = new SipParserMode;
            }
            return instance_->sipParserMode_;
        }
        static SipParserMode& instance() 
        {
            if (!instance_)
            {
                instance_ = new SipParserMode;
            }
            return *instance_;
        }

        void setStrictMode(bool val) { sipParserMode_ = val; };
      
    private:
        bool sipParserMode_;
        static SipParserMode* instance_;
        SipParserMode();

};

}

#endif
