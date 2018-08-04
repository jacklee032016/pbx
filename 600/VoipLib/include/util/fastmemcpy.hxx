#ifndef FASTMEMCPY_HXX_
#define FASTMEMCPY_HXX_
/*
 * $Id: fastmemcpy.hxx,v 1.2 2007/03/01 20:04:29 lizhijie Exp $
 */

#ifndef WIN32
#define USE_DUFFS_DEVICE 1
#define USE_MEMCPY 0
#define USE_FASTMEMCPY 0
#else
#define USE_DUFFS_DEVICE 0
#define USE_MEMCPY 1
#define USE_FASTMEMCPY 0
#endif

#ifndef INLINE_
#define INLINE_ inline
#endif

#if USE_FASTMEMCPY
INLINE_ void fastmemcopy(char* dest, char* src, int count)
{
    long long* s = reinterpret_cast < long long* > (src);
    long long* d = reinterpret_cast < long long* > (dest);
    int numberLongWords = (count + 7) / 8;

    while (numberLongWords--)
    {
        *(d++) = *(s++);
    }
}
#endif

#if USE_DUFFS_DEVICE
INLINE_ void* fastmemcopy(char* dest, char* src, int len)
{
    const long long* s = reinterpret_cast < const long long* > (src);
    long long* d = reinterpret_cast < long long* > (dest);

    int numberLongWords = (len + 7) / 8;

    int count = (numberLongWords + 7) / 8;

    switch (numberLongWords % 8)
    {
        case 0:
        do
        {
            *(d++) = *(s++);
            case 7:
            *(d++) = *(s++);
            case 6:
            *(d++) = *(s++);
            case 5:
            *(d++) = *(s++);
            case 4:
            *(d++) = *(s++);
            case 3:
            *(d++) = *(s++);
            case 2:
            *(d++) = *(s++);
            case 1:
            *(d++) = *(s++);
        }
        while (--count > 0);
    }
    return dest;
}
#endif

#if USE_MEMCPY
#define fastmemcopy(a,b,c) memcpy(a,b,c)
#endif


INLINE_ void mymemcopy(char* dest, const char* src, int count)
{
    memcpy(dest, src, count);
}

#endif
