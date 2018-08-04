#ifndef _SIP_PARSER_MODE_HXX_
#define _SIP_PARSER_MODE_HXX_
/*
 * $Id: SipParserMode.hxx,v 1.2 2007/03/01 20:04:28 lizhijie Exp $
 */

#include "Data.hxx"

namespace Vocal
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

} // namespace Vocal

#endif
