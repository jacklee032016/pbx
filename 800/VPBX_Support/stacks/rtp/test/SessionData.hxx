#ifndef SESSIONDATA_HXX_
#define SESSIONDATA_HXX_

/*
* $Log: SessionData.hxx,v $
* Revision 1.1.1.1  2006/11/30 16:27:09  lizhijie
* AS800 VPBX_Support
*
* Revision 1.1.1.1  2006/06/03 11:55:33  lizhijie
* build a independent directory for VPBX support libraris
*
* $Id: SessionData.hxx,v 1.1.1.1 2006/11/30 16:27:09 lizhijie Exp $
*/

#include "RtpSession.hxx"
#include "VTime.hxx"
#include <string>
#include <deque>

class SessionData
{
    public:
        SessionData();
        void periodicProcess();
        int getFd();
        void handleQueue();
        void processControl();
        void transmitPacket();
        void receivePacket();
        void addQueue(const string& str);
        void newStack(const string& localPort,
                      const string& remoteHost,
                      const string& remotePort );
        void closeStack();
        void shouldUseQueue(bool val);
        void stopRecord();
        void setPktSize(int size);
        void setRecordFile(const string& filename);
        void setPlayFile(const string& filename);

    private:

        RtpSession* stack;
        string playFile;
        int playFileFd;
        int recFileFd;

        deque < string > queueList;

        int state;

        int pktSize;

        VTime lastTimePktSent;
};

#endif
