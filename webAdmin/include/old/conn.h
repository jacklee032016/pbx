/* conn.h
 *
 * Copyright 2004,Philippe Stellwag <linux@mp3s.name>, Germany
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Publice License,
 * version 2 or any later.
 */

#ifndef FAIL
#define FAIL (-1) /* routine failed */
#endif

#define NOHOST (FAIL-1)    /* no such host */
#define NOSERVICE (FAIL-2) /* no such service */

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifdef USG /* brain-dead USG compilers can't deal with typedef */
    #define bcopy(a,b,c) memcpy(b,a,c)
    #ifndef M_XENIX
        #ifndef dgux
            #define u_long unsigned long
            #define u_short unsigned short
        #endif
    #endif
#endif

#ifdef EXCELAN
    #define NONETDB
    #define OLDSOCKET
#endif

#ifdef NONETDB
    #define IPPORT_NNTP 119 /* NNTP is on TCP port 119 */
#endif
