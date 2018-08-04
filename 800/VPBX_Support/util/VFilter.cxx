/*
* $Id: VFilter.cxx,v 1.1.1.1 2006/11/30 16:27:10 lizhijie Exp $
*/

#include "global.h"
#include "VFilter.hxx"
#include "cpLog.h"

#ifndef __vxworks

VFilter* VFilter::instance_ = 0;

VFilter::VFilter()
        :
        inPipe("sip-inpipe"),
        outPipe("sip-outpipe"),
        useFilter(false)
{
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    inPipe.setTimeout(timeout);
    outPipe.setTimeout(timeout);
    if (getenv("SIP_FILTER"))
    {
        cpLog( LOG_INFO, "Environment variable SIP_FILTER is set." );
        cpLog( LOG_INFO, "Using VFilter to modify SIP messages" );
        useFilter = true;
    }
    else
    {
        cpLog( LOG_INFO, "Environment variable SIP_FILTER is Not set." );
        cpLog( LOG_INFO, "Not using VFilter to modify SIP messages" );
    }
}


string VFilter::inFilter(const string& input)
{
    if (!instance_)
    {
        instance_ = new VFilter;
    }

    if (instance_->useFilter)
    {
        instance_->inPipe.clearCtl();

        string output;
        if (instance_->inPipe.sendMsg(input))
        {
            if (instance_->inPipe.recMsg(&output))
            {
                return output;
            }
        }
    }

    // in this case, there was an error or the environment variable
    // is not set, so no filtering is done
    return input;
}


string VFilter::outFilter(const string& input)
{
    if (!instance_)
    {
        instance_ = new VFilter;
    }

    if (instance_->useFilter)
    {
        instance_->outPipe.clearCtl();

        string output;
        if (instance_->outPipe.sendMsg(input))
        {
            if (instance_->outPipe.recMsg(&output))
            {
                return output;
            }
        }
    }

    // in this case, there was an error or the environment variable
    // is not set, so no filtering is done
    return input;
}

#else
// the vxworks

VFilter::VFilter()
{}


string VFilter::inFilter(const string& input)
{
    return input;
}

string VFilter::outFilter(const string& input)
{
    return input;
}

#endif
