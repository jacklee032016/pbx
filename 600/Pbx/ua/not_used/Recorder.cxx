
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
static const char* const Recorder_cxx_Version =
    "$Id: Recorder.cxx,v 1.1.1.1 2006/11/29 09:50:50 lizhijie Exp $";


#include "global.h"
#include "Recorder.h"
#include "cpLog.h"

///
Recorder::Recorder()
{
    m_bActive = false;
}

///
Recorder::~Recorder()
{
    m_bActive = false;
}

/// Start the recording 
int Recorder::start()
{
    cpDebug(LOG_DEBUG, "Record active");
    m_bActive = true;
    return true;
}

/// write the buffer 
int Recorder::write(const void *buffer, int size)
{
    if ( !m_bActive ) return 0;
 #if 0   
    return ::sf_write_raw(m_iFd, (void *)buffer, size);
#endif
	return 0;    
}

/// close the recording 
int Recorder::close()
{
    m_bActive = false;
#if 0    
    return ::sf_close(m_iFd);
#endif
	return 0;    
}


/// Open a file for writing 
int Recorder::open(const string &fileName)
{

    SF_INFO info;

    memset(&info, 0, sizeof(info));
    info.samplerate = 8000;
    info.channels = 1;
#ifdef SNDFILE_1 /* libsndfile 1.0 and above */
    info.format = SF_FORMAT_WAV | SF_FORMAT_ULAW | SF_FORMAT_PCM_U8;
#else
    info.pcmbitwidth = 8;
    info.format = SF_FORMAT_WAV | SF_FORMAT_ULAW;
#endif

#if 0
#ifdef SNDFILE_1 /* libsndfile 1.0 and above */
    m_iFd = sf_open(fileName.c_str(), SFM_WRITE, &info);
#else
    m_iFd = sf_open_write(fileName.c_str(), &info);
#endif
    if ( m_iFd == NULL )
    {
        cpLog(LOG_ERR, "Error opening file for writing");
        return false;
    }
#endif    
    return true;
}
