/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */

static const char* const CJTime_cxx_Version =
    "$Id: CJTime.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $";

/* 
 * You can find out standard defines with 
 *  touch foo.c ; gcc -E -c -dM foo.c
 * gcc seems to have 
 *   __i686__ __unix__ __linux__
 *
 */

/* on sun get
#define __GCC_NEW_VARARGS__ 1 
#define __GNUC_MINOR__ 90 
#define __GNUC__ 2 
#define __SVR4 1 
#define __sparc 1 
#define __sparc__ 1 
#define __sun 1 
#define __sun__ 1 
#define __svr4__ 1 
#define __unix 1 
#define __unix__ 1 
#define sparc 1 
#define sun 1 
#define unix 1 
 */

/* on linux PII get
#define __linux__ 1 
#define linux 1 
#define __i386__ 1 
#define __GNUC_MINOR__ 90 
#define i386 1 
#define i686 1 
#define __unix 1 
#define __unix__ 1 
#define __i686__ 1 
#define __GNUC__ 2 
#define __linux 1    
#define __ELF__ 1 
#define unix 1 
*/

/* on linux PII with pgcc i get 
#define __linux__ 1 
#define linux 1 
#define __i386__ 1 
#define __i386 1 
#define __i686 1 
#define __GNUC_MINOR__ 91 
#define pentiumpro 1 
#define __pentiumpro 1 
#define i386 1 
#define i686 1 
#define __pentiumpro__ 1 
#define __unix 1 
#define __unix__ 1 
#define __i686__ 1 
#define __GNUC__ 2 
#define __linux 1 
#define __ELF__ 1 
#define unix 1 
*/

/* on windows WIN32 */

#include "global.h"

#if defined (__unix__) || defined(__svr4__) || defined (__SUNPRO_CC) || defined(__APPLE__)
  #include <sys/time.h>
  #include <unistd.h>
#else
  #define GLUE_HXX
  #define NO_MIN_MAX_DEF
  #include <windows.h>
  #include "winbase.h"
#endif

/*
#if !defined(__unix__) && !defined(_SUNPRO_CC)
#  define GLUE_HXX
#  define NO_MIN_MAX_DEF
#  include <windows.h>
#  include "winbase.h"
#else
#  include <sys/time.h>
#  include <unistd.h>
#endif
*/
// -ctam
// #include "Glue.hxx"

#include <cassert>
#include <iostream>

// -ctam
// #include "CjMath.hxx"



void 
cjGetTimeOfDay( struct timeval* now )
{
   assert( now );

#if defined(WIN32) || defined(__i686__)
   int cpuSpeedMHz = 450; // TODO - should write a timeing loop to calculate
   int us=0,sec=0;
# ifdef __GNUC__

   unsigned long long time; /* 64 bit */ 
   asm("rdtsc" : "=A" (time));

   time /= cpuSpeedMHz;
   assert( sizeof(time) == 64/8 );
   
   us = ( time % 1000000 );
   sec = (time/1000000);		
# else
   volatile unsigned int lowTime=0,highTime=0;

   __asm
      {
	 rdtsc 
	    mov lowTime, eax
	    mov highTime, edx
	    }
   unsigned __int64 time = highTime;
   time <<= 32;
   time |= lowTime;
   time /= cpuSpeedMHz;

   us = ( time % 1000000 );
   sec = (time/1000000);			
# endif
   now->tv_sec = sec;
   now->tv_usec = us;

#else

#if defined (__unix__) || defined(__svr4__) || defined (__SUNPRO_CC) || defined (__APPLE__)
   gettimeofday( now , NULL );
# else
   SYSTEMTIME t;
   GetSystemTime( &t );
   now->tv_usec = t.wMilliseconds*1000;
   now->tv_sec = (t.wHour*60+t.wMinute)*60+t.wSecond; 
# endif

#endif
}
