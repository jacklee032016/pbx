/*
 * $Id: vsock.cxx,v 1.2 2007/03/01 20:04:33 lizhijie Exp $
 */


#include "global.h"
#include "vsock.hxx"

#ifdef __vxworks
struct hostent* gethostbyname( const char* name )
{
    char buf[1024];
    return (hostent*)resolvGetHostByName( const_cast < char* > (name),
                                          buf, 1024 );
}

struct hostent* gethostbyaddr( const char* addr, int len, int type )
{
    char buf[1024];  // must be at least 512 bytes
    return (hostent*)resolvGetHostByAddr( addr, buf, 1024 );
}

unsigned int getpid( void )
{
    return (unsigned int)taskIdSelf();
}

#endif  // __vxworks


char* getUser( void )
{

#ifndef __vxworks
#ifdef WIN32
    static char user[64] = "unknown_user";
    unsigned long userSize = sizeof(user);
    GetUserName(user, &userSize);
    return (char*)user;

#else
    static char user[64] = "unknown_user";
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    if (pw != NULL)
    {
        strncpy(user, pw->pw_name, sizeof(user));
        user[63] = (char)0;
    }
    return (char*)user;
#endif
#else 
return (char*)sysBootParams.usr;
#endif  //  __vxworks
}


#ifdef __vxworks
int strcasecmp(const char* str1, const char* str2)
{
    unsigned char res;

    while (1)
    {
        if ( (res = (int)tolower(*str1) - (int)tolower(*str2++)) != 0 ||
                !*str1++ )
            break;
    }
    return res;
}


int strncasecmp(const char* str1, const char* str2, size_t len)
{
    unsigned char c1 = 0, c2 = 0;

    if ( len )
    {
        do
        {
            c1 = *str1++;
            c2 = *str2++;
            if (!c1) break;
            if (!c2) break;
            if ( c1 == c2 ) continue;
            c1 = tolower( c1 );
            c2 = tolower( c2 );
            if ( c1 != c2 ) break;
        }
        while (--len);
    }

    return (int)c1 - (int)c2;
}
#endif  //  __vxworks

