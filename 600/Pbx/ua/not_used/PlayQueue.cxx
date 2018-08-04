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

static const char* const PlayQueue_cxx_Version =
    "$Id: PlayQueue.cxx,v 1.2 2007/03/03 18:39:28 lizhijie Exp $";

#include "global.h"
#include "PlayQueue.h"
#include <unistd.h>
#include <fcntl.h>
#include "cpLog.h"
#include <errno.h>


PlayQueue::PlayQueue()
{
    m_bActive = false;
}


PlayQueue::~PlayQueue()
{
    m_bActive = false;
}


void PlayQueue::add(const string & fileName)
{
    cpDebug(LOG_DEBUG, "added %s to play queue", fileName.c_str());
    m_xFileQueue.push(fileName);
}


int PlayQueue::start()
{
    if( m_bActive )
    {
	return true;
    }
    m_bActive = true;
#if 0
    while ( !m_xFileQueue.empty() )
    {
        SF_INFO info;
        string fileName = m_xFileQueue.front();
        m_xFileQueue.pop();
	cpDebug(LOG_DEBUG,"Next file in queue %s",fileName.c_str());
#ifdef SNDFILE_1 /* libsndfile 1.0 and above */
        m_iFd = ::sf_open(fileName.c_str(), SFM_READ, &info);
#else
        m_iFd = ::sf_open_read(fileName.c_str(), &info);
#endif
	if ( m_iFd != NULL )
	{
	    return m_bActive;
	}
	else
	{
	    cpLog(LOG_ERR, "cannot open file %s: %d", fileName.c_str(), errno);
	    m_bActive = false;
	    return false;
	}
    }
#endif    
    cpDebug(LOG_DEBUG, "finished playing queue");
    m_bActive = false;
    return false;
}


int PlayQueue::getData(void *buffer, int size)
{
    if ( !m_bActive ) 
    {
	//cpDebug(LOG_DEBUG, "tried to get data when not playing");
	return true;
    }
#if 0
    int rc = ::sf_read_raw(m_iFd, buffer, size);

    if ( rc != size )
    {
	cpDebug(LOG_DEBUG, "finished reading sound file");
        if ( ((char *)buffer)[rc] == 0 )
	{
	    ((char *)buffer)[rc] = 0x7F;
	}
        memset((char *)buffer + rc, 0x7F, size - rc);
        ::sf_close(m_iFd);
	m_bActive=false;
        return start();
    }
#endif

    return true;
}


void PlayQueue::stop()
{
    m_bActive = false;
    while ( !m_xFileQueue.empty() ) 
    {
	m_xFileQueue.pop();
    }
}

