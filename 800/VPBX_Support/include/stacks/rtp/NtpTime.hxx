#ifndef NTP_HXX
#define NTP_HXX
/*
* $Log: NtpTime.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: NtpTime.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include <sys/types.h>
#include "vtypes.h"

///
class NtpTime
{
    public:
        ///
        NtpTime( u_int32_t sec, u_int32_t frac ): seconds(sec), fractional(frac)
        {}
        ;
        ///
        NtpTime()
        {
            *this = getNtpTime();
        };
        ///
        u_int32_t getSeconds()
        {
            return seconds;
        };
        ///
        u_int32_t getFractional()
        {
            return fractional;
        };
        ///
        void print();
    private:
        u_int32_t seconds;
        u_int32_t fractional;

        friend NtpTime operator+( const NtpTime& rhs , const unsigned int delayMs );
        friend NtpTime operator-( const NtpTime& rhs , const unsigned int ms );
        friend int operator-( const NtpTime& rhs , const NtpTime& lhs );
        friend bool operator==( const NtpTime& rhs , const NtpTime& lhs );
        friend bool operator<( const NtpTime& rhs , const NtpTime& lhs );
        friend bool operator>( const NtpTime& rhs , const NtpTime& lhs );
        friend NtpTime getNtpTime();
};


#endif // NTP_HXX
