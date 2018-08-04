#ifndef VNAMEDPIPEFC_HXX_
#define VNAMEDPIPEFC_HXX_
/*
 * $Id: VNamedPipeFC.hxx,v 1.2 2007/03/01 20:04:30 lizhijie Exp $
 */


#ifndef __vxworks

// unix implementation
#include <assert.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <strstream>
#include <stdio.h>
#include <sys/time.h>


class VNamedPipeFC
{
    public:
        VNamedPipeFC(const char* pipeName);
        bool sendMsg(const string& msg);
        bool recMsg(string* msg);
        void clearCtl();
        void clearTimeout();
        void setTimeout(const struct timeval& timeout);
        ~VNamedPipeFC();

    private:
        int inFd;
        int inCtlFd;
        int outFd;
        int outCtlFd;
        struct timeval localTimeout;
        bool timeoutValid;
        string pipeName_;

        VNamedPipeFC();
        string encodeMsg(const string& msg);
        bool writeOkBlockOrTimeout();
        bool writeOkNoBlock();
        bool writeOk(struct timeval* timeout);
        bool readOkBlockOrTimeout();
        bool readOkNoBlock();
        bool readOk(struct timeval* timeout);
        bool handleInCtl();
};

#else

// vxworks implementation

#endif

#endif
