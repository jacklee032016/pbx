#ifndef NTP_HXX
#define NTP_HXX
/*
 * $Id: NtpTime.hxx,v 1.2 2007/03/01 20:04:27 lizhijie Exp $
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
