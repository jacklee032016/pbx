/*
* $Id: rtpSimple.cxx,v 1.2 2007/03/02 07:29:48 lizhijie Exp $
*/

#include <unistd.h>
#include <fstream>

#include "rtpTypes.h"
#include "Rtp.hxx"
#include "Rtcp.hxx"
#include "RtpSession.hxx"
#ifdef __vxworks
#include "VThread.hxx"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <deque>
#include "SessionData.hxx"
#include "support.hxx"

class MultiSession
{
    public:
        MultiSession();
        void processControl();
        void periodicProcess();
        int setFds(fd_set* fdSet);
        void processFds(fd_set* fdSet);

    private:
        deque < SessionData* > sessionList;
        string cmdBuf;
};


MultiSession::MultiSession()
{
    SessionData* session;
    session = new SessionData;

    sessionList.push_back(session);
}


bool cmd_verify(const split_t& command_list,
                const char* cmd_name,
                unsigned int args)
{
    if (command_list.size() == args)
    {
        if ((command_list[0] == cmd_name))
        {
            return true;
        }
    }
    return false;
}


void MultiSession::processControl()
{
    cout << "got bytes" << endl;
    // this should do something
    char buf[1024];

    int bytesRead = read(0, buf, 1023);

    buf[bytesRead] = '\0';
    cmdBuf += buf;
    // chomp out to the \n

    SessionData* session = sessionList.front();

    while (1)
    {
        string::size_type pos = cmdBuf.find("\n");
        if (pos != string::npos)
        {
            // this is a command
            string cmd;
            cmd = cmdBuf.substr(0, pos);
            split_t myCmd = split(cmd, " ");
            if (cmd_verify(myCmd, "n", 4))
            {
                cout << "new handle" << endl;
                session->newStack(myCmd[1], myCmd[2], myCmd[3]);
            }
            if (myCmd[0] == "v")
            {
                // voice mail mode:
                // play first filename (greeting)
                // play second filename (beep)
                // record third filename
                split_t::iterator i = myCmd.begin();
                i++;
                while (i != myCmd.end())
                {
                    session->addQueue(*i);
                    i++;
                }
                session->shouldUseQueue(true);
            }
            if (cmd_verify(myCmd, "r", 2))
            {
                session->setRecordFile(myCmd[1]);
            }
            if (cmd_verify(myCmd, "sr", 1))
            {
                session->stopRecord();
            }
            if (cmd_verify(myCmd, "p", 2))
            {
                // play file
                session->setPlayFile(myCmd[1]);
            }
            if (cmd_verify(myCmd, "d", 1))
            {
                // delete stack
                session->closeStack();
            }
            if (cmd_verify(myCmd, "s", 2))
            {
                session->setPktSize(atoi(myCmd[1].c_str()));
            }
            if (myCmd[0] == "q")
            {
                session->closeStack();
                exit(0);
            }

            if ((pos + 1) < cmdBuf.length())
            {
                cmdBuf
                = cmdBuf.substr(pos + 1,
                                cmdBuf.length() - (pos + 1));
            }
            else
            {
                cmdBuf = "";
            }
        }
        else
        {
            // no more cmd -- skip
            break;
        }
    }
}


void MultiSession::periodicProcess()
{
    SessionData* session = sessionList.front();
    session->periodicProcess();
}


int MultiSession::setFds(fd_set* fdSet)
{
    int maxFd = 0;

    FD_SET(0, fdSet);   // stdin -- needed for stuff

    SessionData* session = sessionList.front();
    int sessionFd = session->getFd();
    if (sessionFd >= 0)
    {
        FD_SET(sessionFd, fdSet);
        maxFd = sessionFd;
    }
    return maxFd;
}


void MultiSession::processFds(fd_set* fdSet)
{
    if (FD_ISSET(0, fdSet))
    {
        processControl();
    }

    SessionData* session = sessionList.front();
    int sessionFd = session->getFd();
    if (sessionFd >= 0)
    {
        // handle selects
        if (FD_ISSET(sessionFd, fdSet))
        {
            session->receivePacket();
        }
    }
}


void mainAudioLoop()
{
    MultiSession mySession;

    // in the audio stack, handle appropriate stuff
    while (1)
    {
        fd_set readfds;

        struct timeval tv;
        int retval;
        int maxFd = 0;

        // reset file descriptor
        FD_ZERO(&readfds);

        maxFd = mySession.setFds(&readfds);
        maxFd++;

        tv.tv_sec = 0;
        tv.tv_usec = 1000;

        retval = select(maxFd, &readfds, 0, 0, &tv);

        if (retval < 0)
        {
            cout << "select() returned with an error\n";
            return ;
        }

        if (retval > 0)
        {
            mySession.processFds(&readfds);
        }

        // check for too much time elapsing -- do I need to send a packet?

        mySession.periodicProcess();
    }
}



int main (int argc, char *argv[])
{
    mainAudioLoop();

    //    stack.transmitRTCPBYE();

    //    RtpTransmitter* tran = stack.getRtpTran();

    return 0;
}
